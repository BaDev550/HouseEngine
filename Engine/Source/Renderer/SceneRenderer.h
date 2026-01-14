#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Utilities/Memory.h"
#include "World/Entity/Entity.h"
#include "Camera.h"
#include <array>

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

	void DrawScene(std::unordered_map<UUID, Entity>& entities, const MEM::Ref<Camera>& cam);
private:
	struct CameraUniformData {
		glm::mat4 View;
		glm::mat4 Proj;
	} _CameraUD;

	MEM::Scope<VulkanBuffer> _CameraUB;
	VkDescriptorSet _CameraDS;
};

