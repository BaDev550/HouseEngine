#pragma once
#include "Project.h"

namespace House {
	class ProjectSerializer {
	public:
		ProjectSerializer(MEM::Ref<Project>& project);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

		static bool CreateProjectDirectory(const std::string& name, const std::filesystem::path& dir);
		static bool CreateProjectByConfig(const ProjectConfig& config, const std::filesystem::path& path);
	private:
		MEM::Ref<Project> _Project;
	};
}