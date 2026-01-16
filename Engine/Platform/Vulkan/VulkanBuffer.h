#pragma once
#include "VulkanContext.h"
#include "Renderer/Buffer.h"

namespace House {
	namespace Utils {
		VkBufferUsageFlags BufferTypeToVulkanUsageFlag(BufferFlags flags);
		VkMemoryPropertyFlags MemoryPropertiesToVulkanMemoryProperites(MemoryFlags flags);
	}
	class VulkanBuffer : public Buffer
	{
	public:
		static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

		VulkanBuffer(uint64_t size, BufferFlags typeFlags, MemoryFlags memprops);
		~VulkanBuffer();
		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&) = delete;
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		VkBuffer GetBuffer() const { return _Buffer; }

		virtual void Map(uint64_t size = UINT64_MAX, uint64_t offset = 0) override;
		virtual void Unmap() override;

		virtual void WriteToBuffer(void* data, uint64_t size = UINT64_MAX, uint64_t offset = 0) override;
		virtual bool Invalidate(uint64_t size = UINT64_MAX, uint64_t offset = 0) override;
		virtual void* GetData() const override { return _Data; }
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
}