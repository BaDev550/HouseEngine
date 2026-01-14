#pragma once
#include <memory>
#include <vector>
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanTexture.h"
#include "Renderer/Renderer.h"
#include "Renderer/SceneRenderer.h"
#include "Core/Window.h"
#include "Core/Memory.h"
#include "Core/ImGuiLayer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define APPLICATIONNAME "Application"

class Application
{
public:
	Application();
	~Application();

	static Application* Get() { return _Instance; }
	void Run();
public:
	Window& GetWindow() { return *_Window; }
	VulkanContext& GetVulkanContext() { return *_VulkanContext; }
	float GetDeltaTime() const { return _DeltaTime; }
	float GetFPS() const { return 1 / _DeltaTime; }
private:
	static Application* _Instance; // BUG FIX WITH; sawapchain, destroy

	MEM::Scope<Window> _Window;
	MEM::Scope<VulkanContext> _VulkanContext;
	MEM::Scope<SceneRenderer> _SceneRenderer; // Move to world class;
	std::vector<MEM::Ref<Object>> _Objects; // Move to world class

	MEM::Ref<Camera> _Camera;
	MEM::Ref<Object> _CameraObject;
	float _DeltaTime;
};