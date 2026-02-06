#pragma once 
#include <map>
#include <filesystem>
#include "Asset.h"

namespace House {
	static std::map<std::filesystem::path, AssetType> s_ExtensionAssetMap = {
		{ ".hscene", AssetType::Scene },
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".tga", AssetType::Texture },
		{ ".bmp", AssetType::Texture },
		
		{ ".fbx", AssetType::MeshSource },
		{ ".obj", AssetType::MeshSource },
		{ ".gltf", AssetType::MeshSource },

		{ ".hmesh", AssetType::StaticMesh },
		{ ".hmat", AssetType::Material }
	};
}