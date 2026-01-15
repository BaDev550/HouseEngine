#pragma once
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanTexture.h"
#include "Utilities/Memory.h"
#include <functional>
#include <iostream>
#include <array>
#include <map>

enum class RenderPassInputType : uint8_t {
	None = 0,
	UniformBuffer,
	Sampler2D
};

struct RenderPassInput {
	RenderPassInputType Type = RenderPassInputType::None;
	MEM::Ref<MEM::RefCounted> Input;

	RenderPassInput(MEM::Ref<VulkanBuffer> buffer)
		: Type(RenderPassInputType::UniformBuffer), Input(buffer)
	{
	}
	RenderPassInput(MEM::Ref<VulkanTexture> texture)
		: Type(RenderPassInputType::Sampler2D), Input(texture)
	{
	}

	void Set(MEM::Ref<VulkanBuffer> buffer) {
		Type = RenderPassInputType::UniformBuffer;
		Input = buffer;
	}
};

struct RenderPassInputDeclaration {
	RenderPassInputType Type = RenderPassInputType::None;
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
	DescriptorAllocator();

	void WriteInput(std::string_view name, MEM::Ref<VulkanBuffer>  buffer);
	void WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index = 0);

	VkDescriptorSet Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout);
	MEM::Ref<VulkanDescriptorPool>& GetPool() { return _Pool; }
	const std::vector<VkDescriptorSet>& GetDescriptorSets(uint32_t frameIndex) const;
private:
	MEM::Ref<VulkanDescriptorPool> _Pool;
	std::map<uint32_t, std::map<uint32_t, RenderPassInput>> _InputResources;
	std::map<std::string, RenderPassInputDeclaration> _InputDeclarations;
	std::vector<std::vector<VkDescriptorSet>> _DescriptorSets;
	DescriptorAllocatorSpecification _Specs;

	const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name) const;
};

