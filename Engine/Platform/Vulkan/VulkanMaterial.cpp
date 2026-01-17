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

        _DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            _DescriptorSets[i] = _DescriptorManager.Allocate(layout);
            VulkanDescriptorWriter writer(*layout, *_DescriptorManager.GetPool());
            
            VkDescriptorImageInfo diffInfo = _Data.DiffuseTexture.As<VulkanTexture>()->GetImageDescriptorInfo();
            VkDescriptorImageInfo normInfo = _Data.NormalTexture.As<VulkanTexture>()->GetImageDescriptorInfo();

            writer.WriteImage(0, &diffInfo);
            writer.WriteImage(1, &normInfo);
            writer.Overwrite(_DescriptorSets[i]);
        }
    }

    void House::VulkanMaterial::Bind()
    { 
        auto cmd = dynamic_cast<VulkanRenderAPI*>(Renderer::GetAPI())->GetCurrentCommandBuffer();
        uint32_t frameIndex = Renderer::GetFrameIndex();
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline->GetPipelineLayout(), 1, 1, &_DescriptorSets[frameIndex], 0, nullptr);
    }
}