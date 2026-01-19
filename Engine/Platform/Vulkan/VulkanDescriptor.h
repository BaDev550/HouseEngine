#pragma once

#include "VulkanContext.h"
#include "Utilities/Memory.h"
#include <unordered_map>
#include <memory>
#include <vector>

namespace House {
	class VulkanDescriptorSetLayout : public MEM::RefCounted {
	public:
		class Builder {
		public:
			Builder() {}
			Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
			Builder& SetBindingFlags(VkDescriptorBindingFlags flags);
			Builder& SetLayoutFlags(VkDescriptorSetLayoutCreateFlags flags);
			MEM::Ref<VulkanDescriptorSetLayout> Build() const;
		private:
			VkDescriptorSetLayoutCreateFlags _LayoutFlags = 0;
			VkDescriptorBindingFlags _BindingFlags = 0;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _Bindings{};
		};
	public:
		VulkanDescriptorSetLayout(std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings, VkDescriptorBindingFlags bindingFlags, VkDescriptorSetLayoutCreateFlags layoutFlags);
		~VulkanDescriptorSetLayout();
		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
		VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return _DescriptorSetLayout; }
	private:
		VulkanContext& _Context;
		VkDescriptorSetLayout _DescriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _Bindings;
		friend class VulkanDescriptorWriter;
	};

	class VulkanDescriptorPool : public MEM::RefCounted {
	public:
		class Builder {
		public:
			Builder() {}
			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);
			MEM::Ref<VulkanDescriptorPool> Build() const;
		private:
			uint32_t _MaxSets = 1000;
			std::vector<VkDescriptorPoolSize> _PoolSizes{};
			VkDescriptorPoolCreateFlags _PoolFlags = 0;
		};
	public:
		VulkanDescriptorPool(uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
		~VulkanDescriptorPool();
		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

		VkDescriptorPool GetDescriptorPool() const { return _DescriptorPool; }

		bool Allocate(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void Free(std::vector<VkDescriptorSet>& descriptors);
		void Reset();
	private:
		VulkanContext& _Context;
		VkDescriptorPool _DescriptorPool;
		friend class VulkanDescriptorWriter;
	};

	class VulkanDescriptorWriter : public MEM::RefCounted {
	public:
		VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool);
		VulkanDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo, uint32_t count = 1);
		VulkanDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t count = 1);
		VulkanDescriptorWriter& WriteImageArray(uint32_t binding, uint32_t arrayIndex, VkDescriptorImageInfo* imageInfos);
		VulkanDescriptorWriter& WriteBufferArray(uint32_t binding, uint32_t arrayIndex, VkDescriptorBufferInfo* bufferInfos);
		bool Build(VkDescriptorSet& set);
		void Overwrite(VkDescriptorSet& set);
		void Clear();
	private:
		VulkanDescriptorSetLayout& _SetLayout;
		VulkanDescriptorPool& _Pool;
		std::vector<VkWriteDescriptorSet> _Writes;
	};
}