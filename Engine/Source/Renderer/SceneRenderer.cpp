#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

namespace House {
	SceneRenderer::SceneRenderer(MEM::Ref<Scene>& scene)
		: _Scene(scene)
	{
		_MainRenderPass = RenderPass::Create(Renderer::GetPipeline("MainShader"));
		VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
		_CameraUB = Buffer::Create(camerabufferSize, BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
		_CameraUB->Map();
		_MainRenderPass->SetInput("camera", _CameraUB);
	}

	SceneRenderer::~SceneRenderer()
	{
	}

	void SceneRenderer::DrawScene(const MEM::Ref<Camera>& cam)
	{
		_MainRenderPass->Begin();
		
		_CameraUD.View = cam->GetView();
		_CameraUD.Proj = cam->GetProjection();
		_CameraUB->WriteToBuffer(&_CameraUD);

		auto view = _Scene->GetRegistry().view<TransformComponent, StaticMeshComponent>();
		for (auto entity : view) {
			auto& transform = view.get<TransformComponent>(entity);
			auto& model = view.get<StaticMeshComponent>(entity);
			glm::mat4 transformMatrix = transform.ModelMatrix();
			Renderer::DrawMesh(_MainRenderPass, model.Handle, transformMatrix);
		}
		_MainRenderPass->End();
	}
}