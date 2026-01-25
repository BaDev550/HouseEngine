#pragma once 
#include "AssetManagerBase.h"

namespace House {
	class AssetManagerEditor : public AssetManagerBase {
	public:
		AssetManagerEditor();
		~AssetManagerEditor();
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual MEM::Ref<Asset> GetMemoryAsset(AssetHandle handle) override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
		AssetMetadata GetMetadata(AssetHandle handle) const;
		AssetMetadata GetMetadata(const std::filesystem::path& path);
		AssetMap GetLoadedAssets() const;
		AssetMap GetMemoryAssets() const;
		AssetMap GetLoadedAssetsWithType(AssetType type);
		AssetHandle ImportAsset(const std::filesystem::path& path);
		bool IsMemoryAsset(AssetHandle handle) const;
		void AddMemoryOnlyAsset(MEM::Ref<Asset> asset);

		void SaveAssetRegistry();
		void LoadAssetRegistry();

		inline const std::filesystem::path GetAssetPath(const std::filesystem::path& path);
		inline const std::filesystem::path GetAssetPath(const AssetMetadata metadata);
		inline const std::filesystem::path GetAssetRegistryPath();

		template<typename T, typename... Args>
		MEM::Ref<T> Create(const std::string& filename, Args&&... args) {
			static_assert(std::is_base_of<Asset, T>::value, "T Must inherit from asset");
			if (AssetMetadata loadedMetadata = GetMetadata(filename); loadedMetadata.IsValid()) {
				MEM::Ref<Asset> asset = GetAsset(loadedMetadata.Handle);
				_LoadedAssets[loadedMetadata.Handle] = asset;
				return loadedMetadata.Handle;
			}
			AssetMetadata metadata;

			metadata.Handle = UUID();
			metadata.FilePath = GetAssetPath(filename);
			metadata.LoadingState = AssetLoadingState::Loaded;
			metadata.Type = T::GetStaticAssetType();
			_AssetRegistry[metadata.Handle] = metadata;

			MEM::Ref<T> asset = MEM::Ref<T>::Create(std::forward<Args>(args)...);
			asset->Handle = metadata.Handle;
			_LoadedAssets[asset->Handle] = asset;
			AssetImporter::Serialize(metadata, asset);
			SaveAssetRegistry();
			return asset;
		}
	private:
		AssetRegistry _AssetRegistry;
		AssetMap _LoadedAssets;
		AssetMap _MemoryAssets;

		const std::filesystem::path _RegistryFilePath = "AssetRegistry.hfile";
	};
}