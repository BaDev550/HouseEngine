#include "hepch.h"
#include "Core/Application.h"
#include "World/KeyboardMovement.h"
#include <stdexcept>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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
	Renderer::Init();

	_SceneRenderer = MEM::MakeScope<SceneRenderer>();

	_Camera = MEM::MakeRef<Camera>();
	_CameraObject = Object::Create("");
	_Camera->_Position = glm::vec3(0.0f, 3.0f, 0.0f);
	_Objects.push_back(Object::Create("Resources/Models/mario_2/mario_2.obj"));
}

Application::~Application()
{
	_SceneRenderer = nullptr;
	Renderer::Destroy();
	_Window = nullptr;
	_VulkanContext = nullptr;
}

void Application::Run()
{
	Keyboard_Movement mv;
	while (!_Window->ShouldClose()) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		//mv.MoveInPlaneXZ(_Window->GetHandle(), _DeltaTime, *_CameraObject);
		//_Camera->_Position = _CameraObject->_transform.position;
		//_Camera->_Direction = _CameraObject->_transform.rotation;
		_Camera->Update();

		_Window->PollEvents();

		_SceneRenderer->DrawScene(_Objects, _Camera);
	}
	_VulkanContext->WaitToDeviceIdle();
}