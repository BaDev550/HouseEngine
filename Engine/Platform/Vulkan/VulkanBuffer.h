#pragma once
#include "VulkanContext.h"

class VulkanBuffer
{
public:
	static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	VulkanBuffer(VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags mempropFlags, VkDeviceSize minOffsetAlignment = 1);
	~VulkanBuffer();
	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer& operator=(VulkanBuffer&) = delete;

	VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void Unmap();

	void WriteToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult Invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void* GetData() const { return _Data; }
	VkBuffer GetBuffer() const { return _Buffer; }
private:
	void* _Data = nullptr;
	uint32_t _InstanceCount;
	VkBuffer _Buffer;
	VkDeviceMemory _BufferMemory;
	VkDeviceSize _BufferSize;
	VkDeviceSize _InstanceSize;
	VkDeviceSize _AligmentSize;
	VkBufferUsageFlags _UsageFlags;
	VkMemoryPropertyFlags _MemoryPropertyFlags;
	VulkanContext& _Context;
};

