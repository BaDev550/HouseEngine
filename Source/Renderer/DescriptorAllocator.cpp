#include "hepch.h"
#include "DescriptorAllocator.h"
#include "Core/Application.h"

DescriptorAllocator::DescriptorAllocator()
{
	_Pool = VulkanDescriptorPool::Builder()
		.SetMaxSets(1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.Build();
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
