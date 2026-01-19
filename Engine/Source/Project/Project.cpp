#include "hepch.h"
#include "Project.h"
#include "ProjectSerializer.h"

namespace House {
	MEM::Ref<Project> Project::New()
	{
		_ActiveProject = MEM::Ref<Project>::Create();
		ProjectSerializer serializer(_ActiveProject);
		serializer.CreateProjectDirectory("NewProject", "../../");
		_ActiveProject->_ProjectDirectory = "../../NewProject";
		_ActiveProject->_ProjectFilePath = "../../NewProject/NewProject.hproj";
		return _ActiveProject;
	}

	MEM::Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		MEM::Ref<Project> project = MEM::Ref<Project>::Create();
		ProjectSerializer serializer(project);

		if (serializer.Deserialize(path))
		{
			project->_ProjectDirectory = path.parent_path();
			project->_ProjectFilePath = path;
			_ActiveProject = std::move(project);
			return _ActiveProject;
		}
		return nullptr;
	}

	bool Project::SaveActive()
	{
		ProjectSerializer serializer(_ActiveProject);
		if (serializer.Serialize(_ActiveProject->_ProjectFilePath))
			return true;
		return false;
	}
}