#pragma once 
#include "Utilities/UUID.h"
#include "Utilities/Memory.h"

namespace House {
#define INVALID_ASSET_HANDLE 0
#define INVALID_ASSET_TYPE AssetType::None
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
	public:
		AssetHandle Handle;

		static AssetType GetStaticAssetType() { return INVALID_ASSET_TYPE; }
		virtual AssetType GetAssetType() const { return GetStaticAssetType(); }
	};
}