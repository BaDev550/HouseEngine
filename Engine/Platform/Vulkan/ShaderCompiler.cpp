#include "hepch.h"
#include "ShaderCompiler.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <shaderc/shaderc.hpp>
#include "spirv_reflect.h"

namespace House {
    static std::filesystem::path s_ShaderDirectory;

    // Forked from https://github.com/beaumanvienna/vulkan/blob/617f70e1a311c6f498ec69507dcc9d4aadb86612/engine/platform/Vulkan/VKshader.cpp
    class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
        shaderc_include_result* GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth) override;
        void ReleaseInclude(shaderc_include_result* data) override;
        static std::string ReadFile(const std::string& filepath);
    };

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
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:  return ShaderReflectionDataType::Sampler2D;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return ShaderReflectionDataType::UniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return ShaderReflectionDataType::UniformBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:        return ShaderReflectionDataType::StorageBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return ShaderReflectionDataType::StorageBuffer;
            default:
                return ShaderReflectionDataType::UniformBuffer;
            }
        }
        constexpr uint32_t GetFormatSize(VkFormat format) {
            switch (format)
            {
            case VK_FORMAT_UNDEFINED: return 0;
            case VK_FORMAT_R16_SFLOAT: return 2;
            case VK_FORMAT_R16G16_SFLOAT: return 4;
            case VK_FORMAT_R16G16B16_SFLOAT: return 8;
            case VK_FORMAT_R16G16B16A16_SFLOAT: return 16;
            case VK_FORMAT_R32_SFLOAT: return 4;
            case VK_FORMAT_R32G32_SFLOAT: return 8;
            case VK_FORMAT_R32G32B32_SFLOAT: return 12;
            case VK_FORMAT_R32G32B32A32_SFLOAT: return 16;
            }
        }

        ShaderUniformType GetShaderUniformType(const SpvReflectTypeDescription* typeDesc) {
            if (!typeDesc) return House::ShaderUniformType::None;

            if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL) {
                return House::ShaderUniformType::Bool;
            }
            else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
                return House::ShaderUniformType::Int;
            }
            else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
                if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_MATRIX) {
                    if (typeDesc->traits.numeric.matrix.column_count == 3) return House::ShaderUniformType::Mat3;
                    if (typeDesc->traits.numeric.matrix.column_count == 4) return House::ShaderUniformType::Mat4;
                }
                else if (typeDesc->type_flags & SPV_REFLECT_TYPE_FLAG_VECTOR) {
                    if (typeDesc->traits.numeric.vector.component_count == 2) return House::ShaderUniformType::Vec2;
                    if (typeDesc->traits.numeric.vector.component_count == 3) return House::ShaderUniformType::Vec3;
                    if (typeDesc->traits.numeric.vector.component_count == 4) return House::ShaderUniformType::Vec4;
                }
                return House::ShaderUniformType::Float;
            }
            return House::ShaderUniformType::None;
        }
    }

    VkDescriptorType ShaderCompiler::ShaderReflectionTypeToVulkanType(const ShaderReflectionDataType& type)
    {
        switch (type)
        {
        case ShaderReflectionDataType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case ShaderReflectionDataType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        case ShaderReflectionDataType::Sampler2D: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    std::vector<char> ShaderCompiler::CompileShaderFileToSpirv(const std::filesystem::path& path, CompiledShaderInfo& shaderInfo, bool optimize)
    {
        s_ShaderDirectory = path.parent_path();
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
        options.SetIncluder(MEM::MakeScope<ShaderIncluder>());
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_1);
        if (optimize) {
            options.SetOptimizationLevel(shaderc_optimization_level_performance);
        }
        else {
            options.SetOptimizationLevel(shaderc_optimization_level_zero);
        }
        options.SetWarningsAsErrors();

        auto precompileResult = compiler.PreprocessGlsl(source, kind, path.string().c_str(), options);
        if (precompileResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_CORE_ERROR("VK_Shader: Could not preompile shader {0}, error message: {1}", path.string(), precompileResult.GetErrorMessage());
        }

        auto compileResult = compiler.CompileGlslToSpv(source, kind, path.string().c_str(), options);
        if (compileResult.GetCompilationStatus() != shaderc_compilation_status_success)
        {
            LOG_CORE_ERROR("VK_Shader: Could not compile shader {0}, error message: {1}", path.string(), compileResult.GetErrorMessage());
        }

        const auto status = compileResult.GetCompilationStatus();
        if (status != shaderc_compilation_status_success) {
            std::string err = compileResult.GetErrorMessage();
            CHECKF(true, "Shader compile error (" + path.string() + "): " + err)
        }

        const uint32_t* begin = compileResult.cbegin();
        const uint32_t* end = compileResult.cend();
        const size_t word_count = static_cast<size_t>(end - begin);

        std::vector<char> bytes;
        bytes.resize(word_count * sizeof(uint32_t));
        if (word_count > 0) {
            std::memcpy(bytes.data(), reinterpret_cast<const char*>(begin), bytes.size());
        }

        CollectReflectionData(shaderInfo, bytes.data(), bytes.size());

        return bytes;
    }

    void ShaderCompiler::CollectReflectionData(CompiledShaderInfo& shaderInfo, const void* code, size_t sizeInBytes)
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

        uint32_t count;
        spvReflectEnumerateDescriptorBindings(&module, &count, nullptr);
        std::vector<SpvReflectDescriptorBinding*> bindings(count);
        spvReflectEnumerateDescriptorBindings(&module, &count, bindings.data());

        for (const auto& ds : bindings) {
            shaderInfo.ReflectData[ds->set][ds->binding] = { ds->name, ds->count, helpers::GetResourceType(ds) };

            auto& bufferInfo = shaderInfo.Buffers[ds->name];
            bufferInfo.Name = ds->name;
            bufferInfo.Size = ds->block.size;
            LOG_RENDERER_INFO("Found Uniform: {}", bufferInfo.Name);

            for (uint32_t i = 0; i < ds->block.member_count; ++i) {
                const auto& member = ds->block.members[i];

                House::ShaderUniform uniform;
                uniform._Name = member.name;
                uniform._Offset = member.absolute_offset;
                uniform._Size = member.size;
                uniform._Type = helpers::GetShaderUniformType(member.type_description);

                LOG_RENDERER_INFO(" Unifor offset: {}", uniform._Name);
                LOG_RENDERER_INFO(" Unifor size: {}", uniform._Size);
                bufferInfo.Uniforms[uniform._Name] = uniform;
            }
        }

        if (module.shader_stage & SPV_REFLECT_SHADER_STAGE_VERTEX_BIT) {
            shaderInfo.AttribDescriptions.clear();

            spvReflectEnumerateInputVariables(&module, &count, nullptr);
            if (count > 0) {
                std::vector<SpvReflectInterfaceVariable*> inputs(count);
                spvReflectEnumerateInputVariables(&module, &count, inputs.data());
                std::vector<SpvReflectInterfaceVariable*> filteredInputs;
                for (auto* input : inputs) {
                    if (input->location != 0xFFFFFFFF) {
                        filteredInputs.push_back(input);
                    }
                }
                std::sort(filteredInputs.begin(), filteredInputs.end(), [](SpvReflectInterfaceVariable* a, SpvReflectInterfaceVariable* b) { return a->location < b->location; });

                uint32_t stride = 0;
                for (const auto& input : filteredInputs) {
                    VkFormat format = static_cast<VkFormat>(input->format);
                    uint32_t size = helpers::GetFormatSize(format);

                    VkVertexInputAttributeDescription attrib{};
                    attrib.location = input->location;
                    attrib.format = format;
                    attrib.offset = stride;

                    shaderInfo.AttribDescriptions.emplace_back(attrib);
                    stride += size;
                    LOG_RENDERER_INFO("Found input n:{}, l:{}", input->name, attrib.location);
                }
                shaderInfo.BindingDescription.binding = 0;
                shaderInfo.BindingDescription.stride = stride;
                shaderInfo.BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            }
        }

        spvReflectDestroyShaderModule(&module);
    }

    shaderc_include_result* ShaderIncluder::GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth)
    {
        std::string msg = std::string(requestingSource);
        msg += std::to_string(type);
        msg += static_cast<char>(includeDepth);

        const std::string name = std::string(requestedSource);
        const std::string contents = ReadFile(name);

        auto container = new std::array<std::string, 2>;
        (*container)[0] = name;
        (*container)[1] = contents;

        auto data = new shaderc_include_result;

        data->user_data = container;

        data->source_name = (*container)[0].data();
        data->source_name_length = (*container)[0].size();

        data->content = (*container)[1].data();
        data->content_length = (*container)[1].size();

        return data;
    }
    void ShaderIncluder::ReleaseInclude(shaderc_include_result* data)
    {
        delete static_cast<std::array<std::string, 2>*>(data->user_data);
        delete data;
    }
    std::string ShaderIncluder::ReadFile(const std::string& filepath)
    {
        std::string sourceCode;
        std::ifstream in(s_ShaderDirectory / filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size > 0)
            {
                sourceCode.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&sourceCode[0], size);
            }
            else
            {
                LOG_CORE_WARN("ShaderIncluder::ReadFile: Could not read shader file '{0}'", filepath);
            }
        }
        else
        {
            LOG_CORE_WARN("ShaderIncluder::ReadFile Could not open shader file '{0}'", filepath);
        }
        return sourceCode;
    }
}