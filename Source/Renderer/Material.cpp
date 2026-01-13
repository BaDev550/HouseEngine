#include "hepch.h"
#include "Material.h"

Material::Material(MEM::Ref<VulkanPipeline>& pipeline)
	: _Pipeline(pipeline)
{

}

void Material::Build() {
	_MaterialVariables.MaterialDescriptorSet = Renderer::AllocateMaterialSet();
	VkDescriptorImageInfo diffuseInfo = _MaterialVariables.DiffuseTexture->GetImageDescriptorInfo();
	VkDescriptorImageInfo normalInfo = _MaterialVariables.NormalTexture->GetImageDescriptorInfo();

	VulkanDescriptorWriter writer(*Renderer::GetMaterialDescriptorLayout(), *Renderer::GetDescriptorPool());
	
	writer.WriteImage(0, &diffuseInfo);
	writer.WriteImage(1, &normalInfo);
	writer.Overwrite(_MaterialVariables.MaterialDescriptorSet);
}

void Material::Bind(VkCommandBuffer cmd, VkPipelineLayout layout)
{
	_Pipeline->Bind(cmd);
	vkCmdBindDescriptorSets(
		cmd, 
		VK_PIPELINE_BIND_POINT_GRAPHICS, 
		layout, 
		1, 1, 
		&_MaterialVariables.MaterialDescriptorSet, 
		0, 
		nullptr
	);
}