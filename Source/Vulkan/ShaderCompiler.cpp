#include "ShaderCompiler.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <shaderc/shaderc.hpp>

namespace {

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

    std::string ReadFileToString(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("Failed to open shader file: " + path.string());
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }
}

namespace pipeline::utils {
    std::vector<char> CompileShaderFileToSpirv(const std::filesystem::path& path, bool optimize) {
        if (path.extension() == ".spv") {
            std::ifstream in(path, std::ios::binary);
            if (!in) throw std::runtime_error("Failed to open .spv file: " + path.string());
            std::vector<char> data((std::istreambuf_iterator<char>(in)),
                std::istreambuf_iterator<char>());
            return data;
        }

        const std::string source = ReadFileToString(path);
        shaderc::Compiler compiler;
        if (!compiler.IsValid()) {
            throw std::runtime_error("shaderc::Compiler failed to initialize");
        }
        shaderc_shader_kind kind = InferShaderKind(path);
        shaderc::CompileOptions options;

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
            throw std::runtime_error("Shader compile error (" + path.string() + "): " + err);
        }

        const uint32_t* begin = result.cbegin();
        const uint32_t* end = result.cend();
        const size_t word_count = static_cast<size_t>(end - begin);

        std::vector<char> bytes;
        bytes.resize(word_count * sizeof(uint32_t));
        if (word_count > 0) {
            std::memcpy(bytes.data(), reinterpret_cast<const char*>(begin), bytes.size());
        }

        return bytes;
    }
}