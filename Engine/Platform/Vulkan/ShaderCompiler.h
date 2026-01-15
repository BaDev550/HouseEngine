#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <map>

enum class ShaderReflectionDataType : uint8_t {
	None = 0,
	UniformBuffer,
	Sampler2D
};

struct DescriptorInfo {
    std::string Name;
    ShaderReflectionDataType Type;
};

class ShaderCompiler {
public:
    static std::vector<char> CompileShaderFileToSpirv(const std::filesystem::path& path, std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& reflectData, bool optimize = false);
    static void CollectReflectionData(std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& reflectData, const void* code, size_t sizeInBytes);
};
