#pragma once
#include "Utilities/Memory.h"
#include <map>

namespace House {
	enum class ShaderReflectionDataType : uint8_t {
		None = 0,
		UniformBuffer,
		Sampler2D
	};

	struct DescriptorInfo {
		std::string Name;
		ShaderReflectionDataType Type;
	};

	class Shader : public MEM::RefCounted {
	public:
		static MEM::Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath);
		virtual std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& GetReflectData() = 0;
	};
}