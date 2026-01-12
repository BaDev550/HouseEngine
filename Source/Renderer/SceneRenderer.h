#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Core/Memory.h"
#include <array>

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();
private:
	struct CameraUniformData {
		glm::mat4 View;
		glm::mat4 Proj;
	} _CameraUD;

	MEM::Scope<VulkanBuffer> _CameraUB;
	VkDescriptorSet _CameraDS; // Move this to uniform buffer class
};

