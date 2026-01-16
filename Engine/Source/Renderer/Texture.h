#pragma once
#include "Utilities/Memory.h"

namespace House {
	class Texture : public MEM::RefCounted {

	};

	class Texture2D : public Texture {
	public:
		static MEM::Ref<Texture2D> Create(const std::string& path);
		static MEM::Ref<Texture2D> Create(uint32_t* data, uint32_t width, uint32_t height);
	};
}