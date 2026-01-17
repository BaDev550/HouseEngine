#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>

namespace House {
	SceneRenderer::SceneRenderer(MEM::Ref<Scene>& scene)
		: _Scene(scene)
	{
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

		VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
		_CameraUB = Buffer::Create(camerabufferSize, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
		_CameraUB->Map();
		_GRenderPass->SetInput("camera", _CameraUB);
		_FinalImageRenderPass->SetInput("camera", _CameraUB);
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::DrawScene(const MEM::Ref<Camera>& cam)
	{
		_GRenderPass->Begin();
		
		_CameraUD.View = cam->GetView();
		_CameraUD.Proj = cam->GetProjection();
		_CameraUB->WriteToBuffer(&_CameraUD);

		auto view = _Scene->GetRegistry().view<TransformComponent, StaticMeshComponent>();
		for (auto entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
			auto& model = view.get<StaticMeshComponent>(entity);
			glm::mat4 transformMatrix = transform.ModelMatrix();
			Renderer::DrawMesh(_GRenderPass, model.Handle, transformMatrix);
		}
		_GRenderPass->End();

		_FinalImageRenderPass->Begin();
		Renderer::DrawFullscreenQuad(_FinalImageRenderPass);
		_FinalImageRenderPass->End();
	}
}