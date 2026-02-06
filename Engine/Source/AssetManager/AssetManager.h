#pragma once 
#include "Utilities/Memory.h"
#include "Asset.h"
#include "AssetMetadata.h"
#include "AssetManagerEditor.h"

namespace House {
	class AssetManager {
	public:
		static void Init();
		static void Destroy();

		template<typename T, typename... Args>
		static MEM::Ref<T> GetAsset(AssetHandle handle) {
			return s_EditorAssetManager->GetAsset(handle).As<T>();
		}

		template<typename T, typename... Args>
		static MEM::Ref<T> Create(const std::string& filename, Args&&... args) {
			return s_EditorAssetManager->Create<T>(filename, std::forward<Args>(args)...);
		}

		static const AssetMap GetAllLoadedAssetsWithType(AssetType type) { return s_EditorAssetManager->GetLoadedAssetsWithType(type); }
		static MEM::Ref<Asset> GetMemoryAsset(AssetHandle handle) { return s_EditorAssetManager->GetMemoryAsset(handle); }
		static bool IsAssetHandleValid(AssetHandle handle) { return s_EditorAssetManager->IsAssetHandleValid(handle); }
		static bool IsAssetLoaded(AssetHandle handle) { return s_EditorAssetManager->IsAssetLoaded(handle); }
		static AssetType GetAssetType(AssetHandle handle) { return s_EditorAssetManager->GetAssetType(handle); }
		static AssetHandle ImportAsset(const std::string& filename) { return s_EditorAssetManager->ImportAsset(filename); }

		template<typename T>
		static AssetHandle AddMemoryOnlyAsset(MEM::Ref<T> asset) {
			static_assert(std::is_base_of<Asset, T>::value, "T Must inherit from asset");
			asset->Handle = UUID();
			s_EditorAssetManager->AddMemoryOnlyAsset(asset);
			return asset->Handle;
		}
	private:
		static MEM::Scope<AssetManagerEditor> s_EditorAssetManager;
	};
}