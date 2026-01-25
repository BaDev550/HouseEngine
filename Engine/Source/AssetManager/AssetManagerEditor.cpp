#include "hepch.h"
#include "AssetImporter.h"
#include "AssetManagerEditor.h"
#include "AssetExtensions.h"
#include "Project/Project.h"

#include <yaml-cpp/yaml.h>

namespace House {
	AssetManagerEditor::AssetManagerEditor()
	{
		AssetImporter::Init();
		LoadAssetRegistry();
	}

	AssetManagerEditor::~AssetManagerEditor()
	{
		_AssetRegistry.clear();
		_LoadedAssets.clear();
	}

	MEM::Ref<Asset> AssetManagerEditor::GetAsset(AssetHandle handle)
	{
		if (IsMemoryAsset(handle))
			return _MemoryAssets[handle];

		MEM::Ref<Asset> asset;
		AssetMetadata metadata = GetMetadata(handle);
		if (metadata.IsValid()) {
			if (metadata.LoadingState == AssetLoadingState::Loaded) { return _LoadedAssets[handle]; }
			else {
				if (AssetImporter::TryLoadData(metadata, asset)) {
					metadata.LoadingState = AssetLoadingState::Loaded;
					_AssetRegistry[metadata.Handle] = metadata;
					_LoadedAssets[metadata.Handle] = asset;
					SaveAssetRegistry();
					LOG_CORE_INFO("Asset Loaded {}", metadata.FilePath.string());
				}
			}
		}
		return asset;
	}

	MEM::Ref<Asset> AssetManagerEditor::GetMemoryAsset(AssetHandle handle)
	{
		if (auto it = _MemoryAssets.find(handle); it != _MemoryAssets.end())
			return it->second;
		return nullptr;
	}

	bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle) const
	{
		if (_AssetRegistry.find(handle) != _AssetRegistry.end())
			return true;
		return false;
	}

	bool AssetManagerEditor::IsAssetLoaded(AssetHandle handle) const
	{
		if (_LoadedAssets.find(handle) != _LoadedAssets.end())
			return true;
		return false;
	}

	AssetType AssetManagerEditor::GetAssetType(AssetHandle handle) const
	{
		if (IsAssetHandleValid(handle))
			return _AssetRegistry.find(handle)->second.Type;
		return AssetType::None;
	}

	AssetMetadata AssetManagerEditor::GetMetadata(AssetHandle handle) const
	{
		if (_AssetRegistry.find(handle) != _AssetRegistry.end())
			return _AssetRegistry.at(handle);
		return AssetMetadata();
	}

	AssetMetadata AssetManagerEditor::GetMetadata(const std::filesystem::path& path)
	{
		std::filesystem::path assetPath = GetAssetPath(path);
		for (auto& [handle, mtd] : _AssetRegistry) {
			if (mtd.FilePath == assetPath)
				return mtd;
		}
		return AssetMetadata();
	}

	AssetMap AssetManagerEditor::GetLoadedAssets() const
	{
		return _LoadedAssets;
	}

	AssetMap AssetManagerEditor::GetMemoryAssets() const
	{
		return _MemoryAssets;
	}

	AssetMap AssetManagerEditor::GetLoadedAssetsWithType(AssetType type)
	{
		AssetMap resultMap;
		for (auto& [handle, asset] : _LoadedAssets) {
			if (asset && asset->GetAssetType() == type)
				resultMap[handle] = asset;
		}
		return resultMap;
	}

	AssetHandle AssetManagerEditor::ImportAsset(const std::filesystem::path& path)
	{
		if (AssetMetadata loadedMetadata = GetMetadata(path); loadedMetadata.IsValid()) {
			MEM::Ref<Asset> asset = GetAsset(loadedMetadata.Handle);
			_LoadedAssets[loadedMetadata.Handle] = asset;
			return loadedMetadata.Handle;
		}

		std::filesystem::path assetPath = GetAssetPath(path);
		AssetMetadata mtd;
		MEM::Ref<Asset> asset;
		mtd.Handle = UUID();
		mtd.LoadingState = AssetLoadingState::Loading;
		mtd.FilePath = assetPath;
		mtd.Type = s_ExtensionAssetMap[assetPath.extension()];
		if (AssetImporter::TryLoadData(mtd, asset)) {
			mtd.LoadingState = AssetLoadingState::Loaded;
			_AssetRegistry[mtd.Handle] = mtd;
			_LoadedAssets[mtd.Handle] = asset;
			SaveAssetRegistry();
			LOG_CORE_INFO("Asset Loaded {}", mtd.FilePath.string());
			return mtd.Handle;
		}
		return 0;
	}

	bool AssetManagerEditor::IsMemoryAsset(AssetHandle handle) const
	{
		if (_MemoryAssets.find(handle) != _MemoryAssets.end())
			return true;
		return false;
	}

	void AssetManagerEditor::AddMemoryOnlyAsset(MEM::Ref<Asset> asset)
	{
		_MemoryAssets[asset->Handle] = asset;
	}

	void AssetManagerEditor::SaveAssetRegistry()
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Assets" << YAML::BeginSeq;
		for (auto& [handle, entry] : _AssetRegistry) {
			out << YAML::BeginMap;
			out << YAML::Key << "Handle" << YAML::Value << handle;
			out << YAML::Key << "Path" << YAML::Value << entry.FilePath.string();
			out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(entry.Type);
			out << YAML::EndMap;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(GetAssetRegistryPath().string());
		fout << out.c_str();
	}

	void AssetManagerEditor::LoadAssetRegistry()
	{
		if (!std::filesystem::exists(GetAssetRegistryPath()))
			return;

		std::ifstream stream(GetAssetRegistryPath().string());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		auto handles = data["Assets"];
		if (!handles)
			return;

		for (auto entry : handles) {
			std::string filePath = entry["Path"].as<std::string>();
			std::string strType = entry["Type"].as<std::string>();

			AssetMetadata metadata;
			metadata.Handle = entry["Handle"].as<uint64_t>();
			metadata.FilePath = filePath;
			metadata.Type = StringToAssetType(strType);

			if (metadata.Type == AssetType::None) {
				LOG_CORE_WARN("Tried to load AssetType::None asset");
				continue;
			}

			_AssetRegistry[metadata.Handle] = metadata;
		}
	}

	inline const std::filesystem::path AssetManagerEditor::GetAssetPath(const std::filesystem::path& path) { return  (Project::GetCacheDirectory() / path); }
	inline const std::filesystem::path AssetManagerEditor::GetAssetPath(const AssetMetadata metadata) { return  (Project::GetCacheDirectory() / metadata.FilePath); }
	inline const std::filesystem::path AssetManagerEditor::GetAssetRegistryPath() { return (Project::GetCacheDirectory() / _RegistryFilePath); }
}