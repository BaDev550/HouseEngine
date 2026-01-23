#pragma once
#include "AssetSerializer.h"
#include "AssetMetadata.h"

namespace House {
	class AssetImporter {
	public:
		static void Init();
		static void Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset);
		static bool TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset);
	private:
		static std::unordered_map<AssetType, MEM::Scope<AssetSerializer>> s_Serializers;
	};
}