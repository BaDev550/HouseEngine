#pragma once
#include <memory>
#include <vector>
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanTexture.h"
#include "Renderer/Renderer.h"
#include "Core/Window.h"
#include "Core/Memory.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define APPLICATIONNAME "Application"



struct UniformBufferObject {
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Proj;
};

class Application
{
public:
	Application();
	~Application();

	static Application* Get() { return _Instance; }
	void Run();
public:
	Window& GetWindow() { return *_Window; }
	Renderer& GetRenderer() { return *_Renderer; }
	VulkanContext& GetVulkanContext() { return *_VulkanContext; }
private:
	static Application* _Instance;

	MEM::Scope<Window> _Window;
	MEM::Scope<Renderer> _Renderer;
	MEM::Scope<VulkanContext> _VulkanContext;

	float _DeltaTime;
};