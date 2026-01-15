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
	
	void UpdateSets(uint32_t frameIndex);

	VkDescriptorSet Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout);

	MEM::Ref<VulkanDescriptorPool>& GetPool() { return _Pool; }
	VkDescriptorSet& GetDescriptorSet(uint32_t frameIndex, const std::string& name);
	const std::map<std::string, RenderPassInputDeclaration>& GetInputDeclarations() const;
private:
	struct ResourceBinding {
		std::vector<MEM::Ref<MEM::RefCounted>> Data;
		ShaderReflectionDataType Type;
		uint32_t index = 0;

		ResourceBinding(MEM::Ref<VulkanBuffer> buffer) : Data(std::vector<MEM::Ref<MEM::RefCounted>>(1, buffer)), Type(ShaderReflectionDataType::UniformBuffer) {}
		ResourceBinding(MEM::Ref<VulkanTexture> image) : Data(std::vector<MEM::Ref<MEM::RefCounted>>(1, image)),  Type(ShaderReflectionDataType::Sampler2D) {}
	};

	MEM::Ref<VulkanDescriptorPool> _Pool;
	MEM::Ref<VulkanDescriptorWriter> _Writer;

	std::vector<std::vector<VkDescriptorSet>> _DescriptorSets;
	std::map<std::string, RenderPassInputDeclaration> _InputDeclarations;
	std::map<uint32_t, std::map<uint32_t, ResourceBinding>> _StoredResources;
	DescriptorAllocatorSpecification _Specs;

	const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name) const;
};

