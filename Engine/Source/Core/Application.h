#pragma once
#include <memory>
#include <vector>
#include <chrono>

#include "Layers/LayerRegistry.h"
#include "Vulkan/VulkanContext.h"
#include "Layers/ImGuiLayer.h"
#include "Utilities/Memory.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"
#include "Core/Window.h"

struct ApplicationCommandLineArgs {
	int Count = 0;
	char** Args = nullptr;

	const char* operator[](int index) const {
		return Args[index];
	}
};

struct ApplicationSpecs {
	std::string Name;
	std::filesystem::path WorkingDirectory;
	WindowConfig WindowSettings;
	ApplicationCommandLineArgs CommandLineArgs;
};

class Application
{
public:
	Application(const ApplicationSpecs& applicationSpecs);
	~Application();

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	static Application* Get() { return _Instance; }
	void Run();
public:
	Window& GetWindow() { return *_Window; }
	VulkanContext& GetVulkanContext() { return *_Context; }
	ApplicationSpecs GetApplicationSpecs() const { return _ApplicationSpecs; }
	float GetDeltaTime() const { return _DeltaTime; }
	float GetFPS() const { return 1 / _DeltaTime; }
	uint32_t GetFrameIndex() const { return _FrameIndex; }
private:
	static Application* _Instance;

	ApplicationSpecs _ApplicationSpecs;
	LayerRegistry _LayerRegistry;
	ImGuiLayer* _ImGuiLayer;
	MEM::Scope<Window> _Window;
	MEM::Scope<VulkanContext> _Context;

	float _DeltaTime;
	uint32_t _FrameIndex = 0;
};

Application* CreateApplication(ApplicationCommandLineArgs specs);