#include "hepch.h"
#include "ShaderCompiler.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include "spirv_reflect.h"

namespace helpers {

    shaderc_shader_kind InferShaderKind(const std::filesystem::path& path) {
        const auto ext = path.extension().string();
        if (ext == ".vert") return shaderc_vertex_shader;
        if (ext == ".frag") return shaderc_fragment_shader;
        if (ext == ".comp") return shaderc_compute_shader;
        if (ext == ".geom") return shaderc_geometry_shader;
        if (ext == ".tesc") return shaderc_tess_control_shader;
        if (ext == ".tese") return shaderc_tess_evaluation_shader;
        return shaderc_glsl_infer_from_source;
    }

    std::vector<uint32_t> LoadShaderFile(const std::filesystem::path& filePath) {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        const auto fileSize = file.tellg();
        std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();
        return buffer;
    }

    std::string ReadFileToString(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        CHECKF(!in, "Failed to open shader file: " + path.string());
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    constexpr ShaderReflectionDataType GetResourceType(const SpvReflectDescriptorBinding* binding) {
#if 0
        switch (binding->resource_type) {
        case SPV_REFLECT_RESOURCE_FLAG_UNDEFINED: return {};
        case SPV_REFLECT_RESOURCE_FLAG_SAMPLER: return ShaderReflectionDataType::Sampler2D;
        case SPV_REFLECT_RESOURCE_FLAG_CBV: return ShaderReflectionDataType::UniformBuffer;
        }
#endif
        switch (binding->descriptor_type)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return ShaderReflectionDataType::Sampler2D;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return ShaderReflectionDataType::UniformBuffer;
        }
    }
}

std::vector<char> ShaderCompiler::CompileShaderFileToSpirv(const std::filesystem::path& path, std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& reflectData, bool optimize)
{
    if (path.extension() == ".spv") {
        std::ifstream in(path, std::ios::binary);
        CHECKF(!in, "Failed to open .spv file: " + path.string());
        std::vector<char> data((std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());
        return data;
    }

    const std::string source = helpers::ReadFileToString(path);
    shaderc::Compiler compiler;
    CHECKF(!compiler.IsValid(), "shaderc::Compiler failed to initialize");
    shaderc_shader_kind kind = helpers::InferShaderKind(path);
    shaderc::CompileOptions options;

    options.SetGenerateDebugInfo();
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
    if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }
    else {
        options.SetOptimizationLevel(shaderc_optimization_level_zero);
    }
    options.SetWarningsAsErrors();
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(source, kind, path.string().c_str(), options);

    const auto status = result.GetCompilationStatus();
    if (status != shaderc_compilation_status_success) {
        std::string err = result.GetErrorMessage();
        CHECKF(true, "Shader compile error (" + path.string() + "): " + err)
    }

    const uint32_t* begin = result.cbegin();
    const uint32_t* end = result.cend();
    const size_t word_count = static_cast<size_t>(end - begin);

    std::vector<char> bytes;
    bytes.resize(word_count * sizeof(uint32_t));
    if (word_count > 0) {
        std::memcpy(bytes.data(), reinterpret_cast<const char*>(begin), bytes.size());
    }

    CollectReflectionData(reflectData, bytes.data(), bytes.size());

    return bytes;
}

void ShaderCompiler::CollectReflectionData(std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& reflectData, const void* code, size_t sizeInBytes)
{
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(sizeInBytes, code, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    SpvReflectInterfaceVariable** input_vars = (SpvReflectInterfaceVariable**)malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
    result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);

    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    {
        uint32_t count;
        spvReflectEnumerateDescriptorBindings(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorBinding*> bindings(count);
        spvReflectEnumerateDescriptorBindings(&module, &count, bindings.data());
        
        for (const auto& ds : bindings) {
            reflectData[ds->set][ds->binding] = { ds->name, helpers::GetResourceType(ds) };
        }
    }
    spvReflectDestroyShaderModule(&module);
}