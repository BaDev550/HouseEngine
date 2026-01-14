#pragma once
#include "Vulkan/VulkanDescriptor.h"
#include "Utilities/Memory.h"
#include <functional>
#include <iostream>
#include <array>

class DescriptorAllocator
{
public:
	DescriptorAllocator();

	VkDescriptorSet Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout);
	MEM::Ref<VulkanDescriptorPool>& GetPool() { return _Pool; }
private:
	MEM::Ref<VulkanDescriptorPool> _Pool;
};

