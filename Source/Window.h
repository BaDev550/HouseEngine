#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>

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

	GLFWwindow* GetHandle() const { return _Handle; }
	bool ShouldClose() const;
	bool HasResized() const;
	void ResetResizeFlag();
	void PollEvents() const { glfwPollEvents(); }
private:
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	GLFWwindow* _Handle;
	WindowConfig _Config;
};

