#pragma once
#include <iostream>
#include "Utilities/Memory.h"
#include "Utilities/Buffer.h"

namespace House {
	enum class TextureImageFormat {
		None = 0,
		RG16F,
		RG32F,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,

		DEPTH32F,
		DEPTH24STENCIL8
	};

	enum class TextureWrap {
		None = 0,
		Clamp,
		Repeat
	};

	enum class TextureFilter {
		None = 0,
		Linear,
		Nearest
	};

	struct TextureSpecification {
		TextureImageFormat Format = TextureImageFormat::RGBA;
		TextureWrap Wrap = TextureWrap::Repeat;
		TextureFilter Filter = TextureFilter::Linear;
		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t MipLevels = 1;
		bool Attachment = false;
		bool GenerateMipMap = true;
	};

	class Texture : public MEM::RefCounted {

	};

	class Texture2D : public Texture {
	public:
		virtual uint64_t GetImGuiTextureID() = 0;
		static MEM::Ref<Texture2D> Create(const TextureSpecification& spec);
		static MEM::Ref<Texture2D> Create(const TextureSpecification& spec, const std::string& path);
		static MEM::Ref<Texture2D> Create(const TextureSpecification& spec, DataBuffer data);
	};

	inline bool IsDepthFormat(TextureImageFormat format) {
		if (format == TextureImageFormat::DEPTH24STENCIL8 || format == TextureImageFormat::DEPTH32F)
			return true;
		return false;
	}
}