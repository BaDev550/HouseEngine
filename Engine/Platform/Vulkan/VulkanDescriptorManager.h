#pragma once
#include "VulkanDescriptor.h"
#include "VulkanTexture.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"
#include "Utilities/Memory.h"
#include <functional>
#include <iostream>
#include <array>
#include <map>

namespace House {
	struct RenderPassInputDeclaration {
		ShaderReflectionDataType Type = ShaderReflectionDataType::None;
		uint32_t Set = 0;
		uint32_t Binding = 0;
		std::string Name;
	};

	struct DescriptorManagerSpecification {
		MEM::Ref<VulkanShader> Shader;
	};

	class ResourceBinding {
	public:
		std::vector<MEM::Ref<MEM::RefCounted>> Data;
		ShaderReflectionDataType Type = ShaderReflectionDataType::None;

		ResourceBinding() : Data(std::vector<MEM::Ref<MEM::RefCounted>>(1, nullptr)) {}
		ResourceBinding(MEM::Ref<VulkanBuffer> buffer) : Data(std::vector<MEM::Ref<MEM::RefCounted>>(1, buffer)), Type(ShaderReflectionDataType::UniformBuffer) {}
		ResourceBinding(MEM::Ref<VulkanTexture> texture) : Data(std::vector<MEM::Ref<MEM::RefCounted>>(1, texture)), Type(ShaderReflectionDataType::Sampler2D) {}
		ResourceBinding(uint32_t arraySize) : Data(std::vector<MEM::Ref<MEM::RefCounted>>(arraySize, nullptr)), Type(ShaderReflectionDataType::Sampler2D) {}
		void Set(MEM::Ref<VulkanBuffer> buffer, uint32_t index = 0) {
			Data[index] = buffer;
			Type = ShaderReflectionDataType::UniformBuffer;
		}
		void Set(MEM::Ref<VulkanTexture> texture, uint32_t index = 0) {
			Data[index] = texture;
			Type = ShaderReflectionDataType::Sampler2D;
		}
	};

	class DescriptorManager
	{
	public:
		DescriptorManager(DescriptorManagerSpecification& spec);
		DescriptorManager() = default;
		~DescriptorManager();

		void WriteInput(std::string_view name, MEM::Ref<VulkanBuffer>  buffer);
		void WriteInput(std::string_view name, MEM::Ref<VulkanTexture> texture, uint32_t index = 0);

		void UpdateSets(VkCommandBuffer cmd, VkPipelineLayout layout);
		void Invalidate(DescriptorManagerSpecification& spec);

		VkDescriptorSet Allocate(MEM::Ref<VulkanDescriptorSetLayout>& layout);
		VkDescriptorSet GetDescriptorSet(uint32_t frameIndex, uint32_t setIndex);
		std::vector<VkDescriptorSet> GetDescriptorSets(uint32_t frameIndex);
		MEM::Ref<VulkanDescriptorPool>& GetPool() { return _Pool; }
		const std::map<std::string, RenderPassInputDeclaration>& GetInputDeclarations() const;
	private:
		MEM::Ref<VulkanDescriptorPool> _Pool;

		std::vector<std::vector<VkDescriptorSet>> _DescriptorSets;
		std::map<std::string, RenderPassInputDeclaration> _InputDeclarations;
		std::map<uint32_t, std::map<uint32_t, ResourceBinding>> _StoredResources;
		std::map<uint32_t, MEM::Ref<VulkanDescriptorWriter>> _Writers;
		DescriptorManagerSpecification _Specs;

		const RenderPassInputDeclaration* GetInputDeclaration(std::string_view name) const;
	};
}