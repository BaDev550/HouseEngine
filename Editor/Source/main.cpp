#include <iostream>
#include "HouseEditor.h"
#include "Core/Application.h"
#include "Core/EntryPoint.h"

namespace House::Editor {
	class HouseEditor : public Application {
	public:
		HouseEditor(const ApplicationSpecs& applicationSpecs)
			: Application(applicationSpecs)
		{
			PushLayer(new HouseEditorLayer());
		}
	};

}

House::Application* CreateApplication(House::ApplicationCommandLineArgs args) {
	House::ApplicationSpecs specs;
	specs.Name = "HouseEngine";
	specs.WindowSettings.Width = 1280;
	specs.WindowSettings.Height = 720;
	specs.CommandLineArgs = args;
	return new House::Editor::HouseEditor(specs);
}
