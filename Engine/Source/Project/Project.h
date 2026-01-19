#pragma once
#include "Utilities/Memory.h"
#include "Utilities/Defines.h"
#include <string>
#include <filesystem>

namespace House {
	struct ProjectConfig {
		std::string Name;

		std::filesystem::path DefaultScenePath;

		std::filesystem::path AssetDirectory;
		std::filesystem::path CacheDirectory;
		std::filesystem::path ConfigDirectory;

		ProjectConfig() : 
			AssetDirectory("Assets"),
			CacheDirectory("Cache"),
			ConfigDirectory("Config") {}
	};

	class Project : public MEM::RefCounted {
	public:
		static const std::filesystem::path& GetProjectDirectory() {
			CHECKF(!_ActiveProject, "No active project");
			return _ActiveProject->_ProjectDirectory;
		}
		static std::filesystem::path GetAssetDirectory() {
			CHECKF(!_ActiveProject, "No active project");
			return GetProjectDirectory() / _ActiveProject->_Config.AssetDirectory;
		}
		static std::filesystem::path GetCacheDirectory() {
			CHECKF(!_ActiveProject, "No active project");
			return GetProjectDirectory() / _ActiveProject->_Config.CacheDirectory;
		}
		static std::filesystem::path GetConfigDirectory() {
			CHECKF(!_ActiveProject, "No active project");
			return GetProjectDirectory() / _ActiveProject->_Config.ConfigDirectory;
		}

		ProjectConfig& GetConfig() { return _Config; }

		static MEM::Ref<Project> New();
		static MEM::Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive();
	private:
		ProjectConfig _Config;
		std::filesystem::path _ProjectDirectory;
		std::filesystem::path _ProjectFilePath;
		inline static MEM::Ref<Project> _ActiveProject = nullptr;
	};
}