#include "hepch.h"
#include "Core/Application.h"
#include "Utilities/Logger.h"

#include <stdexcept>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Application* Application::_Instance = nullptr;
Application::Application(const ApplicationSpecs& applicationSpecs)
	: _ApplicationSpecs(applicationSpecs)
{
	if (_Instance)
		return;
	_Instance = this;

	Logger::init();
	WindowConfig config{};
	config.Title = _ApplicationSpecs.Name;
	config.Width = _ApplicationSpecs.WindowSettings.Width;
	config.Height = _ApplicationSpecs.WindowSettings.Height;
	
	_Window = MEM::MakeScope<Window>(config);
	_Context = MEM::MakeScope<VulkanContext>();
	Input::Init();
	Renderer::Init();

	_ImGuiLayer = new ImGuiLayer();
	PushOverlay(_ImGuiLayer);
}

Application::~Application()
{
	_LayerRegistry.Clear();
	Renderer::Destroy();
	_Window = nullptr;
	_Context = nullptr;
}

void Application::Run()
{
	bool cursor = true;
	while (!_Window->ShouldClose()) {
		static auto lastFrameTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		_DeltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
		lastFrameTime = currentTime;

		_Window->PollEvents();
		Input::Update();

		if (Input::IsKeyJustPressed(Key::F1)) {
			cursor = !cursor;
			_Window->EnableCursor(cursor);
		}

		auto cmd = Renderer::BeginFrame();
		VulkanCommands::BeginSwapchainRenderPass(cmd);
		for (auto& layer : _LayerRegistry) { layer->OnUpdate(_DeltaTime); }

		_ImGuiLayer->Begin();
		for (auto& layer : _LayerRegistry) { layer->OnImGuiRender(); }
		_ImGuiLayer->End(cmd);

		VulkanCommands::EndSwapchainRenderPass(cmd);
		Renderer::EndFrame();
	}
	_Context->WaitToDeviceIdle();
}