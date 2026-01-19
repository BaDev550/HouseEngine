#pragma once
#include "Scene.h"

namespace House {
	class SceneSerializer {
	public:
		SceneSerializer(const MEM::Ref<Scene>& scene);

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);
	private:
		MEM::Ref<Scene> _Scene;
	};
}