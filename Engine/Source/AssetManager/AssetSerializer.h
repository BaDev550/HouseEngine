#pragma once
#include "AssetMetadata.h"
#include "Asset.h"

namespace House {
	class AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) = 0;
		virtual bool TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) = 0;
		virtual ~AssetSerializer() = default;
	};

	class MeshSourceSerializer : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {};
		virtual bool TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {};
	};

	class MeshSerializer : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {};
		virtual bool TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {};
	};

	class TextureSerializer : public AssetSerializer {
	public:
		virtual void Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {};
		virtual bool TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {};
	};
}