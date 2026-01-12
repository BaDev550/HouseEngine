#include "hepch.h"
#include "Core/Application.h"
#include <stdexcept>

Application* Application::_Instance = nullptr;
Application::Application()
{
	if (_Instance)
		return;
	_Instance = this;

	WindowConfig config{};
	config.Title = APPLICATIONNAME;
	config.Width = 800;
	config.Height = 800;
	
	_Window = MEM::MakeScope<Window>(config);
	_VulkanContext = MEM::MakeScope<VulkanContext>();
	_Renderer = MEM::MakeScope<Renderer>();
}

Application::~Application()
{
	_Renderer = nullptr;
	_Window = nullptr;

	_VulkanContext = nullptr;
}

void Application::Run()
{
	while (!_Window->ShouldClose()) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		_Window->PollEvents();
	}
	_VulkanContext->WaitToDeviceIdle();
}