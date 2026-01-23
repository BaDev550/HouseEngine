#pragma once 
#include "AssetManagerBase.h"

namespace House {
	class AssetManagerEditor : public AssetManagerBase {
	public:
		virtual MEM::Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual MEM::Ref<Asset> GetMemoryAsset(AssetHandle handle) override;
		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		virtual AssetType GetAssetType(AssetHandle handle) const override;
	};
}