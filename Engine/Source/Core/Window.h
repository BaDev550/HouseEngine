#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>

#include "Vulkan/VulkanSwapchain.h"
#include "Vulkan/VulkanContext.h"
#include "Utilities/Memory.h"

struct WindowConfig {
	int Width, Height;
	std::string Title;

	bool Resized = false;

	WindowConfig(int width = 800, int height = 600, const std::string& title = "VulkanProject") : Width(width), Height(height), Title(title) {}
};

class Window {
public:
	Window(const WindowConfig& config);
	~Window();

	uint32_t& GetImageIndex() { return _ImageIndex; }
	GLFWwindow* GetHandle() const { return _Handle; }
	VulkanSwapchain& GetSwapchain() { return *_Swapchain; }
	void CreateSwapchain(VulkanContext* context);
	bool ShouldClose() const;
	bool HasResized() const;
	void ResetResizeFlag();
	void PollEvents() const { glfwPollEvents(); }
	void EnableCursor(bool enabled);
	bool SwapBuffers();
	int GetWidth() const { return _Config.Width; }
	int GetHeight() const { return _Config.Height; }
private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	GLFWwindow* _Handle;
	WindowConfig _Config;
	uint32_t _ImageIndex;
	MEM::Ref<VulkanSwapchain> _Swapchain;
};

