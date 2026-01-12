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
private:
	static Application* _Instance;

	MEM::Scope<Window> _Window;
	MEM::Scope<VulkanContext> _VulkanContext;
	MEM::Scope<SceneRenderer> _SceneRenderer; // Move to world class;

	float _DeltaTime;
};