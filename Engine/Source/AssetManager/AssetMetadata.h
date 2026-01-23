#pragma once 
#include <filesystem>
#include "Asset.h"

namespace House {
	enum class AssetLoadingState {
		NotLoaded = 0,
		Loading,
		Loaded,
		Failed
	};

	struct AssetMetadata {
		AssetHandle Handle;
		AssetType Type = AssetType::None;
		AssetLoadingState LoadingState = AssetLoadingState::NotLoaded;
		std::filesystem::path FilePath;

		bool IsValid() const { return Type != AssetType::None; }
	};
}