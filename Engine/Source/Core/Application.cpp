#include "hepch.h"
#include "Core/Application.h"
#include "Utilities/Logger.h"

#include <stdexcept>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace House {
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
		Input::Init();

		_ImGuiLayer = ImGuiLayer::Create();
		PushOverlay(_ImGuiLayer);

		Renderer::Init();
		LOG_CORE_INFO("Application started");
	}

	Application::~Application()
	{
		LOG_CORE_INFO("Exiting...");
		_Window->GetRenderContext().WaitDeviceIdle();
		
		for (Layer* layer : _LayerRegistry) {
			layer->OnDetach();
			delete layer;
		}

		Renderer::Destroy();
		_Window = nullptr;
		Logger::Destroy();
	}

	void Application::PushLayer(Layer* layer) {
		_LayerRegistry.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		_LayerRegistry.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		while (!_Window->ShouldClose()) {
			static auto lastFrameTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			_DeltaTime = std::chrono::duration<float>(currentTime - lastFrameTime).count();
			lastFrameTime = currentTime;

			_Window->PollEvents();
			Input::Update();

			Renderer::BeginFrame();

			{
				for (auto& layer : _LayerRegistry)
					layer->OnUpdate(_DeltaTime); 
			}

			Renderer::Submit([this]() { DrawImGui(); });
			Renderer::Submit([=]() { _ImGuiLayer->End(); });

			Renderer::WaitAndRender();
			if (Renderer::EndFrame())
				_FrameIndex = (_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
		}
	}
	void Application::DrawImGui()
	{
		_ImGuiLayer->Begin();
		for (auto& layer : _LayerRegistry)
			layer->OnImGuiRender();
	}
}