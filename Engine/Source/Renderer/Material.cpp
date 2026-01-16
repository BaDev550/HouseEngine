#include "hepch.h"
#include "Material.h"

Material::Material(MEM::Ref<VulkanPipeline>& pipeline)
	: _Pipeline(pipeline)
{

}

void Material::Build() {
    auto& manager = Renderer::GetDescriptorManager();
    auto& shader = _Pipeline->GetShader();
    auto& layout = shader->GetDescriptorLayout(1);

    _DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        _DescriptorSets[i] = manager->Allocate(layout);
        VulkanDescriptorWriter writer(*layout, *manager->GetPool());

        VkDescriptorImageInfo diffInfo = _MaterialVariables.DiffuseTexture->GetImageDescriptorInfo();
        VkDescriptorImageInfo normInfo = _MaterialVariables.NormalTexture->GetImageDescriptorInfo();

        writer.WriteImage(0, &diffInfo);
        writer.WriteImage(1, &normInfo);
        writer.Overwrite(_DescriptorSets[i]);
    }
}

void Material::Bind(VkCommandBuffer cmd)
{
	_Pipeline->Bind(cmd);
    uint32_t frameIndex = Renderer::GetFrameIndex();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline->GetPipelineLayout(), 1, 1, &_DescriptorSets[frameIndex], 0, nullptr);
}