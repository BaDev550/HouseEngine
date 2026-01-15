#pragma once
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanPipeline.h"
#include "Utilities/Memory.h"
#include <functional>
#include <iostream>
#include <array>
#include <map>

struct RenderPassInputDeclaration {
	ShaderReflectionDataType Type = ShaderReflectionDataType::None;
	uint32_t Set = 0;
	uint32_t Binding = 0;
	std::string Name;
};

struct DescriptorAllocatorSpecification {
	MEM::Ref<VulkanPipeline> Pipeline;
};

class DescriptorAllocator
{
public:
	DescriptorAllocator(DescriptorAllocatorSpecification& spec);

	void WriteInput(std::string_view name, MEM::Ref<VulkanBuffer>  buffer);
	void WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index = 0);

	VkDescriptorSet Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout);
	MEM::Ref<VulkanDescriptorPool>& GetPool() { return _Pool; }
	VkDescriptorSet& GetDescriptorSet(const std::string& name);
	const std::map<uint32_t, std::map<uint32_t, RenderPassInputDeclaration>>& GetInputDeclarations() const;
private:
	MEM::Ref<VulkanDescriptorPool> _Pool;
	MEM::Ref<VulkanDescriptorWriter> _Writer;

	std::map<std::string, VkDescriptorSet> _DescriptorSets;
	std::map<uint32_t, std::map<uint32_t, RenderPassInputDeclaration>> _InputDeclarations;
	DescriptorAllocatorSpecification _Specs;

	//const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name) const;
};

