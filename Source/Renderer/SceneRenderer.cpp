#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

SceneRenderer::SceneRenderer()
{
	// Camera Uniform buffer set
	{
		VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
		_CameraUB = MEM::MakeScope<VulkanBuffer>(
			camerabufferSize,
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		_CameraUB->Map();
	}

	VkDescriptorBufferInfo cameraBufferInfo = _CameraUB->DescriptorInfo();
	VulkanDescriptorWriter(*Renderer::GetGlobalDescriptorLayout(), *Renderer::GetDescriptorPool())
		.WriteBuffer(0, &cameraBufferInfo)
		.Build(_CameraDS);
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::DrawScene(std::vector<MEM::Ref<Object>>& objects, MEM::Ref<Camera>& cam)
{
	VkCommandBuffer cmd = Renderer::BeginFrame();
	VulkanCommands::BeginSwapchainRenderPass(cmd);

	_CameraUD.View = cam->GetView();
	_CameraUD.Proj = cam->GetProjection();
	_CameraUB->WriteToBuffer(&_CameraUD);
	vkCmdBindDescriptorSets(
		cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		Renderer::GetPipelineLayout(),
		0, 1,
		&_CameraDS,
		0,
		nullptr
	);

	for (auto& object : objects) {
		Renderer::RenderMesh(cmd, Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline"), object->_Model, object->_transform.Mat4());
	}

	VulkanCommands::EndSwapchainRenderPass(cmd);
	Renderer::EndFrame();
}
