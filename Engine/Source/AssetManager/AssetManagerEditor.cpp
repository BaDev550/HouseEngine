#include "hepch.h"
#include "Asset.h"

namespace House {
	std::string AssetTypeToString(AssetType type) {
		switch (type) {
		case AssetType::None: return "None";
		case AssetType::Scene: return "Scene";
		case AssetType::MeshSource: return "MeshSource";
		case AssetType::StaticMesh: return "StaticMesh";
		case AssetType::Texture: return "Texture";
		case AssetType::Material: return "Material";
		default: return "Unknown";
		}
	}

	AssetType StringToAssetType(const std::string& typeStr) {
		if (typeStr == "None") return AssetType::None;
		if (typeStr == "Scene") return AssetType::Scene;
		if (typeStr == "MeshSource") return AssetType::MeshSource;
		if (typeStr == "StaticMesh") return AssetType::StaticMesh;
		if (typeStr == "Texture") return AssetType::Texture;
		if (typeStr == "Material") return AssetType::Material;
		return AssetType::None;
	}
}