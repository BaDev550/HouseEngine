#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

SceneRenderer::SceneRenderer()
{
	// Camera Uniform buffer set
	VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
	_CameraUB = MEM::Ref<VulkanBuffer>::Create(
		camerabufferSize,
		1,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	_CameraUB->Map();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::DrawScene(std::unordered_map<UUID, Entity>& entities, const MEM::Ref<Camera>& cam)
{
	_CameraUD.View = cam->GetView();
	_CameraUD.Proj = cam->GetProjection();
	_CameraUB->WriteToBuffer(&_CameraUD);
	Renderer::GetDescriptorAllocator()->WriteInput("camera", _CameraUB);
	
	for (auto& object : entities) {
		if (!object.second.HasComponent<StaticMeshComponent>())
			continue;


		auto& model = object.second.GetComponent<StaticMeshComponent>();
		glm::mat4 transform = object.second.GetComponent<TransformComponent>().ModelMatrix();
		Renderer::RenderMesh(
			Renderer::GetCurrentCommandBuffer(), 
			Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline"), 
			model.Handle,
			transform
		);
	}
}
