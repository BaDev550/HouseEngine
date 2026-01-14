#include <iostream>
#include "HouseEditor.h"
#include "Core/Application.h"
#include "Core/EntryPoint.h"

class HouseEditor : public Application {
public:
	HouseEditor(const ApplicationSpecs& applicationSpecs)
		: Application(applicationSpecs)
	{
		PushLayer(new HouseEditorLayer());
	}
};


Application* CreateApplication(ApplicationCommandLineArgs args) {
	ApplicationSpecs specs;
	specs.Name = "HouseEngine";
	specs.WindowSettings.Width = 1280;
	specs.WindowSettings.Height = 720;
	specs.CommandLineArgs = args;
	return new HouseEditor(specs);
}