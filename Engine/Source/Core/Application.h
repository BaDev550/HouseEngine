#pragma once
#include <memory>
#include <vector>

#include "Vulkan/VulkanContext.h"
#include "Renderer/Renderer.h"
#include "Renderer/SceneRenderer.h"
#include "Core/Window.h"
#include "Utilities/Memory.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

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

	static Application* Get() { return _Instance; }
	void Run();
public:
	Window& GetWindow() { return *_Window; }
	VulkanContext& GetVulkanContext() { return *_Context; }
	ApplicationSpecs GetApplicationSpecs() const { return _ApplicationSpecs; }
	float GetDeltaTime() const { return _DeltaTime; }
	float GetFPS() const { return 1 / _DeltaTime; }
private:
	static Application* _Instance;

	ApplicationSpecs _ApplicationSpecs;
	MEM::Scope<Window> _Window;
	MEM::Scope<VulkanContext> _Context;

	float _DeltaTime;
};

Application* CreateApplication(ApplicationCommandLineArgs specs);