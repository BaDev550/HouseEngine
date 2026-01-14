#include <iostream>
#include "Core/Application.h"
#include "Core/EntryPoint.h"

Application* CreateApplication(ApplicationCommandLineArgs args) {
	ApplicationSpecs specs;
	specs.Name = "HouseEngine";
	specs.WindowSettings.Width = 1280;
	specs.WindowSettings.Height = 720;
	specs.CommandLineArgs = args;
	return new Application(specs);
}