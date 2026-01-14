#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptor.h"
#include "World/Object.h"
#include "Core/Memory.h"
#include "Core/ImGuiLayer.h"
#include "Camera.h"
#include <array>

class SceneRenderer
{
public:
	SceneRenderer();
	~SceneRenderer();

	void DrawScene(std::vector<MEM::Ref<Object>>& objects, MEM::Ref<Camera>& cam);
private:
	struct CameraUniformData {
		glm::mat4 View;
		glm::mat4 Proj;
	} _CameraUD;

	MEM::Scope<VulkanBuffer> _CameraUB;
	VkDescriptorSet _CameraDS; // Move this to uniform buffer class

	MEM::Scope<ImGuiLayer> _ImGuiLayer;

	Object* _SelectedObject = nullptr;
};

