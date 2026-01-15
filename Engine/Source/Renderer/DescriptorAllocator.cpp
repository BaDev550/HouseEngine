#include "hepch.h"
#include "DescriptorAllocator.h"
#include "Core/Application.h"

DescriptorAllocator::DescriptorAllocator()
{
	_Pool = VulkanDescriptorPool::Builder()
		.SetMaxSets(1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
		.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
		.Build();

	_Specs.Pipeline;
}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanBuffer> buffer)
{
	const RenderPassInputDeclaration* decl = GetInputDeclaration(name);
	if (decl)
		_InputResources.at(decl->Set).at(decl->Binding).Set(buffer);
}

void DescriptorAllocator::WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index)
{
}

VkDescriptorSet DescriptorAllocator::Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout)
{
	VkDescriptorSet set;
	VkDescriptorSetLayout l = layout->GetDescriptorSetLayout();

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _Pool->GetDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &l;

	CHECKF(vkAllocateDescriptorSets(Application::Get()->GetVulkanContext().GetDevice(), &allocInfo, &set) != VK_SUCCESS, "Failed to allocate descriptor set");
	return set;
}

const std::vector<VkDescriptorSet>& DescriptorAllocator::GetDescriptorSets(uint32_t frameIndex) const { return _DescriptorSets[frameIndex]; }
const RenderPassInputDeclaration* DescriptorAllocator::GetInputDeclaration(std::string_view name) const
{
	std::string nameStr(name);
	if (_InputDeclarations.find(nameStr) == _InputDeclarations.end())
		return nullptr;

	const RenderPassInputDeclaration& decl = _InputDeclarations.at(nameStr);
	return &decl;
}
