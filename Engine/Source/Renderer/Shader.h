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

	enum class ShaderUniformType {
		None = 0,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat3,
		Mat4
	};

	class ShaderUniform {
	public:
		ShaderUniform() = default;
		ShaderUniform(std::string name, ShaderUniformType type, uint32_t size, uint32_t offset);

		const std::string& GetName() const { return _Name; }
		const uint32_t& GetSize() const { return _Size; }
		const uint32_t& GetOffset() const { return _Offset; }
	private:
		std::string _Name;
		ShaderUniformType _Type = ShaderUniformType::None;
		uint32_t _Size = 0;
		uint32_t _Offset = 0;

		friend class ShaderCompiler;
	};

	struct ShaderBuffer {
		std::string Name;
		uint32_t Size = 0;
		std::unordered_map<std::string, ShaderUniform> Uniforms;
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