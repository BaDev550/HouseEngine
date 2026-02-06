#include "hepch.h"
#include "VulkanMaterial.h"
#include "VulkanRenderAPI.h"
#include "VulkanTexture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace House {
    VulkanMaterial::VulkanMaterial(MEM::Ref<Shader>& shader)
    {
        _Shader = shader.As<VulkanShader>();

        DescriptorManagerSpecification spec{};
        spec.Shader = _Shader;
        _DescriptorManager.Invalidate(spec);

        Build();
    }

    VulkanMaterial::~VulkanMaterial()
    {
        _StorageBuffer.Release();
    }

    void VulkanMaterial::Build()
    {
        if (_Shader->GetDescriptorLayout(1)) {
            auto& layout = _Shader->GetDescriptorLayout(1);
            const auto& shaderBuffers = _Shader->GetShaderBuffers();

            if (shaderBuffers.size() > 0) {
                uint32_t size = 0;
                for (auto [name, buffer] : shaderBuffers)
                    size += buffer.Size;

                _StorageBuffer.Allocate(size);
                _StorageBuffer.ZeroInitialize();
            }

            _DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                _DescriptorSets[i] = _DescriptorManager.Allocate(layout);
            }

            for (const auto& [name, decl] : _DescriptorManager.GetInputDeclarations()) {
                if (decl.Type == ShaderReflectionDataType::Sampler2D) {
                    _DescriptorManager.WriteInput(name, Renderer::GetWhiteTexture().As<VulkanTexture>());
                }
            }
        }
    }

    void VulkanMaterial::Bind(VkCommandBuffer cmd, VkPipelineLayout layout)
    {
        _DescriptorManager.UpdateSets(cmd, layout);
    }

    void VulkanMaterial::Set(const std::string& name, float value)
    {
        Set<float>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, int value)
    {
        Set<int>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, bool value)
    {
        Set<bool>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec2& value)
    {
        Set<glm::vec2>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec3& value)
    {
        Set<glm::vec3>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec4& value)
    {
        Set<glm::vec4>(name, value);
    }

    void VulkanMaterial::Set(const std::string& name, const MEM::Ref<Texture2D>& value)
    {
        _DescriptorManager.WriteInput(name, value.As<VulkanTexture>());
    }

    float& VulkanMaterial::GetFloat(const std::string& name)
    {
        return Get<float>(name);
    }

    glm::vec2& VulkanMaterial::GetVector2(const std::string& name)
    {
        return Get<glm::vec2>(name);
    }

    glm::vec3& VulkanMaterial::GetVector3(const std::string& name)
    {
        return Get<glm::vec3>(name);
    }

    void VulkanMaterial::MaterialDataChanged()
    {
        //_MaterialBuffer->WriteToBuffer(&_Data);
    }

    const ShaderUniform* VulkanMaterial::FindUniformDeclaration(const std::string& name)
    {
        const auto& shaderbuffers = _Shader->GetShaderBuffers();
        std::string targetUniformName = name;
        std::string targetBufferName = "";

        size_t dotPos = name.find('.');
        if (dotPos != std::string::npos) {
            targetBufferName = name.substr(0, dotPos);
            targetUniformName = name.substr(dotPos + 1);
        }

        for (const auto& [bufferKey, buffer] : shaderbuffers) {
            if (!targetBufferName.empty() && bufferKey != targetBufferName)
                continue;

            auto it = buffer.Uniforms.find(targetUniformName);
            if (it != buffer.Uniforms.end()) {
                return &it->second;
            }
        }

        return nullptr;
    }
}