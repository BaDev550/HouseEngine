#pragma once
#include "Utilities/Memory.h"
#include <map>

namespace House {
	enum class ShaderReflectionDataType : uint8_t {
		None = 0,
		UniformBuffer,
		StorageBuffer,
		Sampler2D
	};

	struct DescriptorInfo {
		std::string Name;
		uint32_t Count = 1;
		ShaderReflectionDataType Type;

		static std::string ShaderReflectionDataTypeToString(ShaderReflectionDataType type) {
			switch (type) {
			case ShaderReflectionDataType::None: return "None";
			case ShaderReflectionDataType::UniformBuffer: return "UniformBuffer";
			case ShaderReflectionDataType::StorageBuffer: return "StorageBuffer";
			case ShaderReflectionDataType::Sampler2D: return "Sampler2D";
			default: return "Unknown";
			}
		}
	};

	class Shader : public MEM::RefCounted {
	public:
		static MEM::Ref<Shader> Create(const std::string& vertexPath, const std::string& fragmentPath);
		virtual std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& GetReflectData() = 0;
	};
}