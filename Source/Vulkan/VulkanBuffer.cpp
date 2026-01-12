#include "hepch.h"
#include "VulkanBuffer.h"
#include "Core/Application.h"

VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
{
    if (minOffsetAlignment > 0)
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    return instanceSize;
}

VulkanBuffer::VulkanBuffer(VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags mempropFlags, VkDeviceSize minOffsetAlignment)
    : _InstanceSize(instanceSize), _InstanceCount(instanceCount), _UsageFlags(usageFlags), _MemoryPropertyFlags(mempropFlags), _Context(*Application::Get()->GetVulkanContext())
{
    _AligmentSize = GetAlignment(instanceSize, minOffsetAlignment);
    _BufferSize = _AligmentSize * instanceCount;
    _Context.CreateBuffer(_BufferSize, _UsageFlags, _MemoryPropertyFlags, _Buffer, _BufferMemory);
}

VulkanBuffer::~VulkanBuffer()
{
    Unmap();
    vkDestroyBuffer(_Context.GetDevice(), _Buffer, nullptr);
    vkFreeMemory(_Context.GetDevice(), _BufferMemory, nullptr);
}

VkResult VulkanBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
{
    if (!_Buffer && !_BufferMemory)
        throw std::runtime_error("Called 'MAP BUFFER' before buffer created");
    return vkMapMemory(_Context.GetDevice(), _BufferMemory, offset, size, 0, &_Data);
}

void VulkanBuffer::Unmap()
{
    if (_Data) {
        vkUnmapMemory(_Context.GetDevice(), _BufferMemory);
        _Data = nullptr;
    }
}

void VulkanBuffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
{
    if (!_Data)
        throw std::runtime_error("Cannot copy to unmapped buffer");
    if (size == VK_WHOLE_SIZE) {
        memcpy(_Data, data, _BufferSize);
    }
    else {
        char* memOffset = (char*)_Data;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkDescriptorBufferInfo VulkanBuffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
{
    return VkDescriptorBufferInfo{ _Buffer, offset, size };
}

VkResult VulkanBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset)
{
    return VkResult();
}
