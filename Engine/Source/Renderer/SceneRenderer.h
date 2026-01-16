#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Utilities/Memory.h"

#include "World/Entity/Entity.h"
#include "World/Scene/Scene.h"
#include "Camera.h"
#include <array>

class SceneRenderer : public MEM::RefCounted
{
public:
	SceneRenderer(MEM::Ref<Scene>& scene);
	~SceneRenderer();

	void DrawScene(const MEM::Ref<Camera>& cam);
private:
	struct CameraUniformData {
		glm::mat4 View;
		glm::mat4 Proj;
	} _CameraUD;

	MEM::Ref<VulkanBuffer> _CameraUB;

	MEM::Ref<Scene> _Scene;
};

