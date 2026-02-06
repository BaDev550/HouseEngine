#include "hepch.h"
#include "AssetManager.h"

namespace House { 
	MEM::Scope<AssetManagerEditor> AssetManager::s_EditorAssetManager = nullptr;

	void AssetManager::Init(){
		s_EditorAssetManager = MEM::MakeScope<AssetManagerEditor>();
	}

	void AssetManager::Destroy() {
		s_EditorAssetManager = nullptr;
	}
}