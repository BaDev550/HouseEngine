#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

SceneRenderer::SceneRenderer(MEM::Ref<Scene>& scene)
	: _Scene(scene)
{
	VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
	_CameraUB = MEM::Ref<VulkanBuffer>::Create(
		camerabufferSize,
		1,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	_CameraUB->Map();
	Renderer::GetDescriptorManager()->WriteInput("camera", _CameraUB);
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::DrawScene(const MEM::Ref<Camera>& cam)
{
	Renderer::GetDescriptorManager()->UpdateSets(Renderer::GetFrameIndex());

	_CameraUD.View = cam->GetView();
	_CameraUD.Proj = cam->GetProjection();
	_CameraUB->WriteToBuffer(&_CameraUD);

	auto view = _Scene->GetRegistry().view<TransformComponent, StaticMeshComponent>();
	for (auto entity : view) {
		auto& transform = view.get<TransformComponent>(entity);
		auto& model = view.get<StaticMeshComponent>(entity);
		glm::mat4 transformMatrix = transform.ModelMatrix();
		Renderer::RenderMesh(
			Renderer::GetCurrentCommandBuffer(),
			Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline"),
			model.Handle,
			transformMatrix
		);
	}
}
