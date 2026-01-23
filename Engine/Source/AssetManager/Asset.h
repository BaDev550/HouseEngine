#pragma once 
#include "Utilities/UUID.h"
#include "Utilities/Memory.h"

namespace House {
	enum class  AssetType {
		None = 0,
		Scene,
		MeshSource,
		StaticMesh,
		Texture,
		Material
	};
	std::string AssetTypeToString(AssetType type);
	AssetType StringToAssetType(const std::string& typeStr);

	using AssetHandle = UUID;
	class Asset : public MEM::RefCounted {
		AssetHandle Handle;

		static AssetType GetStaticAssetType() { return AssetType::None; }
		virtual AssetType GetAssetType() const { return GetStaticAssetType(); }
	};
}