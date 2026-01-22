#include "hepch.h"
#include <fstream>
#include "ProjectSerializer.h"
#include "World/Scene/Scene.h"
#include "World/Scene/SceneSerializer.h"

#include <yaml-cpp/yaml.h>

namespace House {
	ProjectSerializer::ProjectSerializer(MEM::Ref<Project>& project)
		: _Project(project)
	{
	}

	bool ProjectSerializer::Serialize(const std::filesystem::path& path)
	{
		const auto& config = _Project->GetConfig();
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << config.Name;
		out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
		out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
		out << YAML::Key << "ConfigDirectory" << YAML::Value << config.ConfigDirectory.string();
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path& path)
	{
		auto& config = _Project->GetConfig();
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		} catch (YAML::ParserException& e) {
			LOG_CORE_CRITICAL("Failed to load project file: {0}", e.what());
			return false;
		}
		
		auto projectNode = data["Project"];
		if (!projectNode)
			return false;

		config.Name = projectNode["Name"].as<std::string>();
		config.AssetDirectory = projectNode["AssetDirectory"].as<std::string>();
		config.CacheDirectory = projectNode["CacheDirectory"].as<std::string>();
		config.ConfigDirectory = projectNode["ConfigDirectory"].as<std::string>();
		return true;
	}

	bool ProjectSerializer::CreateProjectDirectory(const std::string& name, const std::filesystem::path& dir)
	{
		ProjectConfig config = ProjectConfig();
		
		config.Name = name;
		const char* enginePath = std::getenv("HOUSE_DIR");
		CHECKF(!enginePath, "Enviroment vairable HOUSE_DIR is not.");

		std::filesystem::path projectDir = dir / name;
		std::filesystem::path projectFile = projectDir / (name + ".hproj");
		std::filesystem::path configDir = projectDir / config.ConfigDirectory;
		
		std::string startupScene = "defaultScene";
		MEM::Ref<Scene> scene = MEM::Ref<Scene>::Create(startupScene);
		SceneSerializer sceneSerializer(scene);

		std::filesystem::create_directories(projectDir);
		if (CreateProjectByConfig(config, projectFile)) {
			std::filesystem::create_directories(projectDir / config.AssetDirectory);
			std::filesystem::create_directories(projectDir / config.CacheDirectory);
			std::filesystem::create_directories(projectDir / config.ConfigDirectory);
			sceneSerializer.Serialize(projectDir / config.AssetDirectory / (startupScene + ".scene"));
			return true;
		}
	}

	bool ProjectSerializer::CreateProjectByConfig(const ProjectConfig& config, const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Project" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Name" << YAML::Value << config.Name;
		out << YAML::Key << "AssetDirectory" << YAML::Value << config.AssetDirectory.string();
		out << YAML::Key << "CacheDirectory" << YAML::Value << config.CacheDirectory.string();
		out << YAML::Key << "ConfigDirectory" << YAML::Value << config.ConfigDirectory.string();
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();

		return true;
	}
}