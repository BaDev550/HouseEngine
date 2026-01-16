#include "hepch.h"
#include "Core/Window.h"
#include <assert.h>

namespace House {
	static bool GLFWInitialized = false;
	Window::Window(const WindowConfig& config)
	{
		_Config = config;
		if (!GLFWInitialized) {
			int success = glfwInit();
			assert(success && "Failed to initialize GLFW window");
			GLFWInitialized = true;
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		}
		_Handle = glfwCreateWindow(_Config.Width, _Config.Height, _Config.Title.c_str(), nullptr, nullptr);

		glfwMakeContextCurrent(_Handle);
		glfwSetWindowUserPointer(_Handle, &_Config);
		glfwSetFramebufferSizeCallback(_Handle, FramebufferResizeCallback);
	}

	Window::~Window()
	{
		delete _Swapchain;
		_Swapchain = nullptr;
		glfwDestroyWindow(_Handle);
		glfwTerminate();
	}

	void Window::CreateSwapchain(VulkanContext* context) { _Swapchain = new VulkanSwapchain(context); }
	bool Window::ShouldClose() const { return glfwWindowShouldClose(_Handle); }
	bool Window::HasResized() const { return _Config.Resized; }
	void Window::ResetResizeFlag() { _Config.Resized = false; }
	void Window::EnableCursor(bool enabled)
	{
		glfwSetInputMode(_Handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
	bool Window::SwapBuffers()
	{
		VkResult result = _Swapchain->AcquireNextImage(&_ImageIndex);
		CHECKF((result != VK_SUCCESS || result == VK_SUBOPTIMAL_KHR), "Failed to acquire next image");
		return true;
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowConfig* config = reinterpret_cast<WindowConfig*>(glfwGetWindowUserPointer(window));
		config->Resized = true;
	}
}