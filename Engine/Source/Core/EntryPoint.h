#pragma once

#include "Core/Application.h"
extern House::Application* CreateApplication(House::ApplicationCommandLineArgs specs);

int main(int argc, char** argv) {
	auto app = CreateApplication({ argc, argv });
	app->Run();
	delete app;
	return 0;
}