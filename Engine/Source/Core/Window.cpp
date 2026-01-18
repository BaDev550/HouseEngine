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
		_RenderContext = RenderContext::Create(_Handle);
		_Swapchain = Swapchain::Create(_RenderContext);

		glfwMakeContextCurrent(_Handle);
		glfwSetWindowUserPointer(_Handle, &_Config);
		glfwSetFramebufferSizeCallback(_Handle, FramebufferResizeCallback);
	}

	Window::~Window()
	{
		delete _Swapchain;
		_Swapchain = nullptr;

		delete _RenderContext;
		_RenderContext = nullptr;

		glfwDestroyWindow(_Handle);
		glfwTerminate();
	}

	bool Window::ShouldClose() const { return glfwWindowShouldClose(_Handle); }
	bool Window::HasResized() const { return _Config.Resized; }
	void Window::ResetResizeFlag() { _Config.Resized = false; }
	void Window::EnableCursor(bool enabled)
	{
		glfwSetInputMode(_Handle, GLFW_CURSOR, enabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
	bool Window::SwapBuffers()
	{
		bool result = _Swapchain->Swapbuffers(&_ImageIndex);
		return true;
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		WindowConfig* config = reinterpret_cast<WindowConfig*>(glfwGetWindowUserPointer(window));
		config->Resized = true;
	}
}