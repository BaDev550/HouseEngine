#pragma once

#include "VulkanContext.h"
#include <unordered_map>
#include <memory>
#include <vector>

class VulkanDescriptorSetLayout {
public:
	class Builder {
	public:
		Builder() {}
		Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
		std::unique_ptr<VulkanDescriptorSetLayout> Build() const;
	private:
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> _Bindings{};
	};
public:
	VulkanDescriptorSetLayout(std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
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

class VulkanDescriptorPool {
public:
	class Builder {
	public:
		Builder() {}
		Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& SetMaxSets(uint32_t count);
		std::unique_ptr<VulkanDescriptorPool> Build() const;
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

class VulkanDescriptorWriter {
public:
	VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool);
	VulkanDescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	VulkanDescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
	bool Build(VkDescriptorSet& set);
	void Overwrite(VkDescriptorSet& set);
private:
	VulkanDescriptorSetLayout& _SetLayout;
	VulkanDescriptorPool& _Pool;
	std::vector<VkWriteDescriptorSet> _Writes;
};