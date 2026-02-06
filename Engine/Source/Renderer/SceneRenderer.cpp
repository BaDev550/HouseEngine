#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include "Texture.h"
#include "Utilities/Frustum.h"
#include "World/Components/Components.h"
#include <glm/gtc/type_ptr.hpp>

namespace House {
	SceneRenderer::SceneRenderer(MEM::Ref<Scene>& scene)
		: _Scene(scene)
	{
		// Create uniform buffer objects
		{
			CreateSSAO();

			uint64_t camerabufferSize = sizeof(CameraUniformData);
			_CameraUB = Buffer::Create(camerabufferSize, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
			_CameraUB->Map();

			uint64_t directionalLightBufferSize = sizeof(UniformBufferDirectionalLight);
			_DirectionalLightUB = Buffer::Create(directionalLightBufferSize, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
			_DirectionalLightUB->Map();

			uint64_t pointLightsBufferSize = sizeof(UniformBufferPointLights);
			_PointLightsUB = Buffer::Create(pointLightsBufferSize, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
			_PointLightsUB->Map();
		}

		// Create gbuffer and lightling buffer for deferred rendering
		{
			FramebufferSpecification spec{};
			spec.Attachments = { 
				TextureImageFormat::RGBA16F, 
				TextureImageFormat::RGBA16F,
				TextureImageFormat::RGBA,
				TextureImageFormat::DEPTH32F 
			};
			spec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			spec.DepthClearValue = 1.0f;
			MEM::Ref<Framebuffer> frameBuffer = Framebuffer::Create(spec);

			PipelineData data{};
			data.Framebuffer = frameBuffer;
			data.Shader = Renderer::GetShaderLibrary()->GetShader("PBRStatic");
			_GPipeline = Pipeline::Create(data);
			_GRenderPass = RenderPass::Create(_GPipeline);

			PipelineData lightingData{};
			lightingData.Framebuffer = nullptr; // Use swapchain default framebuffer
			lightingData.Shader = Renderer::GetShaderLibrary()->GetShader("DeferredLighting");
			_FinalImagePipeline = Pipeline::Create(lightingData);
			_FinalImageRenderPass = RenderPass::Create(_FinalImagePipeline);

			_FinalImageRenderPass->SetInput("uPosition", frameBuffer->GetAttachmentTexture(0));
			_FinalImageRenderPass->SetInput("uNormal",   frameBuffer->GetAttachmentTexture(1));
			_FinalImageRenderPass->SetInput("uAlbedo",   frameBuffer->GetAttachmentTexture(2));
		}

		// Create screen space ao render pass
		{
			FramebufferSpecification ssaoSpec{};
			ssaoSpec.Attachments = { TextureImageFormat::R16F };
			MEM::Ref<Framebuffer> ssaoFramebuffer = Framebuffer::Create(ssaoSpec);

			PipelineData ssaoPipelineData;
			ssaoPipelineData.Framebuffer = ssaoFramebuffer;
			ssaoPipelineData.Shader = Renderer::GetShaderLibrary()->GetShader("SSAO");
			MEM::Ref<Pipeline> ssaoPipeline = Pipeline::Create(ssaoPipelineData);
			_SSAO.AOPass = RenderPass::Create(ssaoPipeline);
			_SSAO.AOPass->SetInput("uPosition", _GRenderPass->GetFramebuffer()->GetAttachmentTexture(0));
			_SSAO.AOPass->SetInput("uNormal",   _GRenderPass->GetFramebuffer()->GetAttachmentTexture(1));
			_SSAO.AOPass->SetInput("uTexNoise", _SSAO.NoiseTexture);
			_SSAO.AOPass->SetInput("uKernel",   _SSAO.KernelBuffer);
			_SSAO.AOPass->SetInput("uCamera", _CameraUB);

			MEM::Ref<Framebuffer> ssaoBlurFramebuffer = Framebuffer::Create(ssaoSpec);
			PipelineData blurPipelineData;
			blurPipelineData.Framebuffer = ssaoBlurFramebuffer;
			blurPipelineData.Shader = Renderer::GetShaderLibrary()->GetShader("SSAO_Blur");
			MEM::Ref<Pipeline> ssaoBlurPipeline = Pipeline::Create(blurPipelineData);
			_SSAO.AOBlurPass = RenderPass::Create(ssaoBlurPipeline);
			_SSAO.AOBlurPass->SetInput("ssaoInput", ssaoFramebuffer->GetAttachmentTexture(0));

			_FinalImageRenderPass->SetInput("uSSAO", ssaoBlurFramebuffer->GetAttachmentTexture(0));
		}

		_EndlessGrid = MEM::Ref<EndlessGrid>::Create();

		_GRenderPass->SetInput("uCamera", _CameraUB);
		_FinalImageRenderPass->SetInput("uCamera", _CameraUB);
		_FinalImageRenderPass->SetInput("uDirectionalLight", _DirectionalLightUB);
		_FinalImageRenderPass->SetInput("uPointLights", _PointLightsUB);
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::DrawScene(Camera& cam)
	{
#if 0
		Frustum cameraFrustum;
		cameraFrustum.Update(cam->GetProjection() * cam->GetView());
#endif
		_GRenderPass->Begin();

		_SceneData.CameraData.View = cam.GetView();
		_SceneData.CameraData.Proj = cam.GetProjection();
		_SceneData.CameraData.Position = cam.GetPosition();
		_CameraUB->WriteToBuffer(&_SceneData.CameraData);

		auto view = _Scene->GetRegistry().view<TransformComponent, StaticMeshComponent>();
		for (auto entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
			auto& staticMeshHandle = view.get<StaticMeshComponent>(entity).Handle;
			auto staticMesh = AssetManager::GetAsset<StaticMesh>(staticMeshHandle);
			auto meshSource = AssetManager::GetAsset<MeshSource>(staticMesh->GetMeshSource());

			glm::mat4 transformMatrix = transform.ModelMatrix();
			Renderer::DrawStaticMesh(_GRenderPass, staticMesh, meshSource, transformMatrix);
		}
		_GRenderPass->End();

		_SSAO.AOPass->Begin();
		Renderer::DrawFullscreenQuad(_SSAO.AOPass);
		_SSAO.AOPass->End();

		_SSAO.AOBlurPass->Begin();
		Renderer::DrawFullscreenQuad(_SSAO.AOBlurPass);
		_SSAO.AOBlurPass->End();

		_FinalImageRenderPass->Begin();
		CollectLightDataFromScene();
		UniformBufferDirectionalLight& ubdDirectionalLight = _SceneData.LightEnviromentUniformData.UBDDirectionalLight;
		UniformBufferPointLights& ubdPointLights = _SceneData.LightEnviromentUniformData.UBDPointLights;
		ubdDirectionalLight.Light = _SceneData.LightEnviromentData.DirectionalLight;
		ubdPointLights.Count = _SceneData.LightEnviromentData.GetPointLightCount();
		bool hasPointLight = ubdPointLights.Count > 0;
		if (hasPointLight) std::memcpy(ubdPointLights.PointLights, _SceneData.LightEnviromentData.PointLights.data(), sizeof(PointLight) * ubdPointLights.Count);

		_DirectionalLightUB->WriteToBuffer(&ubdDirectionalLight);
		_PointLightsUB->WriteToBuffer(&ubdPointLights);

		if (_SceneData.DrawGrid) {
			Renderer::DrawVertex(Renderer::GetPipeline("Grid"), _EndlessGrid->_GridBuffer, _EndlessGrid->_VertexCount);
		}

		Renderer::DrawFullscreenQuad(_FinalImageRenderPass);
		_FinalImageRenderPass->End();
	}

	void SceneRenderer::CollectLightDataFromScene()
	{
		auto& lightEnviroment = _SceneData.LightEnviromentData;
		uint32_t pointLightCount = 0;
		uint32_t dirLightCount = 0;
		{
			auto view = _Scene->GetRegistry().view<TransformComponent, DirectionalLightComponent>();
			for (auto entity : view) {
				if (dirLightCount >= 1) break;
				auto& tc = view.get<TransformComponent>(entity);
				auto& dLightC = view.get<DirectionalLightComponent>(entity);

				dLightC.Handle.Direction = glm::normalize(tc.Rotation);
				lightEnviroment.DirectionalLight = dLightC.Handle;
				dirLightCount++;
			}
		}
		{
			auto view = _Scene->GetRegistry().group<PointLightComponent>(entt::get<TransformComponent>);
			lightEnviroment.PointLights.resize(view.size());
			for (auto entity : view) {
				if (pointLightCount >= 1024) break;
				auto& tc = view.get<TransformComponent>(entity);
				auto& pLightC = view.get<PointLightComponent>(entity);

				pLightC.Handle.Position = tc.Position;
				lightEnviroment.PointLights[pointLightCount++] = pLightC.Handle;
			}
		}
	}

	// From learnopengl.com
	void SceneRenderer::CreateSSAO()
	{
		std::uniform_real_distribution<float> randomFloats(0.0, 1.0f);
		std::default_random_engine generator;

		_SSAO.Kernel.clear();
		for (uint32_t i = 0; i < _SSAO.Samples; i++) {
			glm::vec3 sample(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator)
			);
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);

			float scale = float(i) / _SSAO.Samples;
			scale = 0.1f + scale * scale * (1.0f - 0.1f);
			sample *= scale;
			_SSAO.Kernel.push_back(sample);
		}

		_SSAO.KernelBuffer = Buffer::Create(sizeof(glm::vec3) * _SSAO.Samples, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
		_SSAO.KernelBuffer->Map();
		_SSAO.KernelBuffer->WriteToBuffer(_SSAO.Kernel.data());

		std::vector<glm::vec4> ssaoNoise;
		for (unsigned int i = 0; i < 16; i++) {
			glm::vec3 noise(
				randomFloats(generator) * 2.0 - 1.0,
				randomFloats(generator) * 2.0 - 1.0,
				0.0f
			);
			ssaoNoise.push_back(glm::vec4(noise, 1.0));
		}

		TextureSpecification noiseSpec;
		noiseSpec.Width = 4;
		noiseSpec.Height = 4;
		noiseSpec.Format = TextureImageFormat::RGBA32F;
		uint64_t sizeInBytes = ssaoNoise.size() * sizeof(glm::vec4);
		_SSAO.NoiseTexture = Texture2D::Create(noiseSpec, DataBuffer(ssaoNoise.data(), sizeInBytes));
	}
}