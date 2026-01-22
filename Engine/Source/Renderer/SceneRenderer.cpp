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

		// Create uniform buffer objects
		{
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
			auto& model = view.get<StaticMeshComponent>(entity);

			glm::mat4 transformMatrix = transform.ModelMatrix();
			Renderer::DrawMesh(_GRenderPass, model.Handle, transformMatrix);
		}
		_GRenderPass->End();

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

		Renderer::DrawFullscreenQuad(_FinalImageRenderPass);
		_FinalImageRenderPass->End();
	}

	void SceneRenderer::CollectLightDataFromScene()
	{
		auto& lightEnviroment = _SceneData.LightEnviromentData;
		{
			auto view = _Scene->GetRegistry().view<TransformComponent, DirectionalLightComponent>();
			uint32_t dirLightCount = 0;
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
			auto view = _Scene->GetRegistry().view<TransformComponent, PointLightComponent>();
			uint32_t pointLightCount = 0;
			lightEnviroment.PointLights.resize(view.size_hint());
			for (auto entity : view) {
				if (pointLightCount >= 1024) break;
				auto& tc = view.get<TransformComponent>(entity);
				auto& pLightC = view.get<PointLightComponent>(entity);

				pLightC.Handle.Position = tc.Position;
				lightEnviroment.PointLights[pointLightCount++] = pLightC.Handle;
			}
		}
	}
}