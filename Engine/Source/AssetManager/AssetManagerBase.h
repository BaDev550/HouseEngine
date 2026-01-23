#pragma once 
#include "Utilities/UUID.h"
#include "Utilities/Memory.h"
#include "Asset.h"
#include "AssetMetadata.h"

namespace House {
	using AssetMap = std::map<AssetHandle, MEM::Ref<Asset>>;
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	class AssetManagerBase {
	public:
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) = 0;
		virtual MEM::Ref<Asset> GetMemoryAsset(AssetHandle handle) = 0;
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;
		virtual AssetType GetAssetType(AssetHandle handle) const = 0;
	};
}