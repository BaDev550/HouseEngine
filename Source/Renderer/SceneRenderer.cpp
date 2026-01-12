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
	VulkanDescriptorWriter(*Renderer::GetVulkanDescriptorSetLayout(0), *Renderer::GetDescriptorPool())
		.WriteBuffer(0, &cameraBufferInfo)
		.Build(_CameraDS);
}

SceneRenderer::~SceneRenderer()
{
}
