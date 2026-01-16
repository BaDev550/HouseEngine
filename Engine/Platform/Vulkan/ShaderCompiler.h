#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <map>
#include "Renderer/Shader.h"

namespace House {
    struct CompiledShaderInfo {
        std::map<uint32_t, std::map<uint32_t, DescriptorInfo>> ReflectData;
        std::vector<VkVertexInputAttributeDescription> ArrtibDescriptions;
        VkVertexInputBindingDescription BindingDescription{};
    };
    class ShaderCompiler {
    public:
        static VkDescriptorType ShaderReflectionTypeToVulkanType(const ShaderReflectionDataType& type);
        static std::vector<char> CompileShaderFileToSpirv(const std::filesystem::path& path, CompiledShaderInfo& shaderInfo, bool optimize = false);
        static void CollectReflectionData(CompiledShaderInfo& shaderInfo, const void* code, size_t sizeInBytes);
    };
}