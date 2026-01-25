#include "hepch.h"
#include "VulkanMaterial.h"
#include "VulkanRenderAPI.h"
#include "VulkanTexture.h"

namespace House {
    VulkanMaterial::VulkanMaterial(MEM::Ref<Pipeline>& pipeline)
    {
        _Pipeline = pipeline.As<VulkanPipeline>();
        DescriptorManagerSpecification spec{};
        spec.Pipeline = _Pipeline;
        _DescriptorManager.Invalidate(spec);
    }

    void VulkanMaterial::Build()
    {
        auto shader = _Pipeline->GetShader().As<VulkanShader>();
        auto& layout = shader->GetDescriptorLayout(1);

		_MaterialBuffer = MEM::Ref<VulkanBuffer>::Create(sizeof(MaterialData), BufferType::UniformBuffer, MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT);
        _MaterialBuffer->Map();
        _MaterialBuffer->WriteToBuffer(&_Data);

        _DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            _DescriptorSets[i] = _DescriptorManager.Allocate(layout);
            VulkanDescriptorWriter writer(*layout, *_DescriptorManager.GetPool());

            VkDescriptorImageInfo diffInfo = _AlbedoTexture.As<VulkanTexture>()->GetImageDescriptorInfo();
            VkDescriptorImageInfo normInfo = _NormalTexture.As<VulkanTexture>()->GetImageDescriptorInfo();
            VkDescriptorImageInfo metallicRoughnessInfo = _MetallicRoughnessTexture.As<VulkanTexture>()->GetImageDescriptorInfo();
			VkDescriptorBufferInfo materialBufferInfo = _MaterialBuffer->DescriptorInfo();

            writer.WriteImage(0, &diffInfo);
            writer.WriteImage(1, &normInfo);
			writer.WriteImage(2, &metallicRoughnessInfo);
			writer.WriteBuffer(3, &materialBufferInfo);
            writer.Overwrite(_DescriptorSets[i]);
        }
    }

    void House::VulkanMaterial::Bind()
    { 
        auto cmd = Renderer::GetAPI<VulkanRenderAPI>()->GetCurrentCommandBuffer();
        uint32_t frameIndex = Renderer::GetFrameIndex();
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline->GetPipelineLayout(), 1, 1, &_DescriptorSets[frameIndex], 0, nullptr);
    }

    void VulkanMaterial::Set(const std::string& name, float value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, int value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, bool value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec2& value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec3& value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, const glm::vec4& value)
    {
    }

    void VulkanMaterial::Set(const std::string& name, const MEM::Ref<Texture2D>& value)
    {
        //_DescriptorManager.WriteInput(name, value);
    }

    float& VulkanMaterial::GetFloat(const std::string& name)
    {
        // TODO: insert return statement here
    }

    glm::vec2& VulkanMaterial::GetVector2(const std::string& name)
    {
        // TODO: insert return statement here
    }

    glm::vec3& VulkanMaterial::GetVector3(const std::string& name)
    {
        // TODO: insert return statement here
    }

    void VulkanMaterial::MaterialDataChanged()
    {
        _MaterialBuffer->WriteToBuffer(&_Data);
    }
}