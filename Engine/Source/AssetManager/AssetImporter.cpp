#include "hepch.h"
#include "AssetImporter.h"

namespace House {
	std::unordered_map<AssetType, MEM::Scope<AssetSerializer>> AssetImporter::s_Serializers;
	void AssetImporter::Init(){
		s_Serializers.clear();
		s_Serializers[AssetType::MeshSource] = MEM::MakeScope<MeshSourceSerializer>();
		s_Serializers[AssetType::StaticMesh] = MEM::MakeScope<MeshSerializer>();
		s_Serializers[AssetType::Texture] = MEM::MakeScope<TextureSerializer>();
	}
	void AssetImporter::Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset){
		if (s_Serializers.find(metadata.Type) != s_Serializers.end()){
			s_Serializers[metadata.Type]->Serialize(metadata, asset);
		}
		else {
			LOG_CORE_ERROR("No serializer found for asset type {0}", AssetTypeToString(metadata.Type));
		}
	}
	bool AssetImporter::TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {
		if (s_Serializers.find(metadata.Type) != s_Serializers.end()) {
			return s_Serializers[metadata.Type]->TryLoadData(metadata, asset);
		}
		LOG_CORE_ERROR("No serializer found for asset type {0}", AssetTypeToString(metadata.Type));
	}
}