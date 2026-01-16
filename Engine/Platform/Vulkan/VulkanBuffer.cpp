#include "hepch.h"
#include "VulkanBuffer.h"
#include "Core/Application.h"

namespace House {
    namespace Utils {
        VkBufferUsageFlags BufferTypeToVulkanUsageFlag(BufferFlags flags)
        {
            VkBufferUsageFlags vkFlags = 0;
            if (flags.Has(BufferType::UniformBuffer)) vkFlags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            if (flags.Has(BufferType::IndexBuffer))   vkFlags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            if (flags.Has(BufferType::VertexBuffer))  vkFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            if (flags.Has(BufferType::TransferDst))   vkFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            if (flags.Has(BufferType::TransferSrc))   vkFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            return vkFlags;
        }

        VkMemoryPropertyFlags MemoryPropertiesToVulkanMemoryProperites(MemoryFlags flags)
        {
            VkMemoryPropertyFlags vkFlags = 0;
            if (flags.Has(MemoryProperties::HOST_COHERENT)) vkFlags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            if (flags.Has(MemoryProperties::HOST_VISIBLE))  vkFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            if (flags.Has(MemoryProperties::DEVICE))        vkFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            return vkFlags;
        }
    }

    VkDeviceSize VulkanBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
    {
        if (minOffsetAlignment > 0)
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        return instanceSize;
    }

    VulkanBuffer::VulkanBuffer(uint64_t size, BufferFlags typeFlags, MemoryFlags memprops)
        : 
        _InstanceSize(size), 
        _InstanceCount(1), 
        _UsageFlags(Utils::BufferTypeToVulkanUsageFlag(typeFlags)),
        _MemoryPropertyFlags(Utils::MemoryPropertiesToVulkanMemoryProperites(memprops)),
        _Context(Application::Get()->GetVulkanContext())
    {
        _AligmentSize = GetAlignment(size, 1);
        _BufferSize = _AligmentSize * _InstanceCount;
        _Context.CreateBuffer(_BufferSize, _UsageFlags, _MemoryPropertyFlags, _Buffer, _BufferMemory);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        Unmap();
        vkDestroyBuffer(_Context.GetDevice(), _Buffer, nullptr);
        vkFreeMemory(_Context.GetDevice(), _BufferMemory, nullptr);
    }

    void VulkanBuffer::Map(uint64_t size, uint64_t offset)
    {
        CHECKF((!_Buffer && !_BufferMemory), "Called 'MAP BUFFER' before buffer created");
        CHECKF(vkMapMemory(_Context.GetDevice(), _BufferMemory, offset, size, 0, &_Data) != VK_SUCCESS, "Failed to map memory");
    }

    void VulkanBuffer::Unmap()
    {
        if (_Data) {
            vkUnmapMemory(_Context.GetDevice(), _BufferMemory);
            _Data = nullptr;
        }
    }

    bool VulkanBuffer::Invalidate(uint64_t size, uint64_t offset) { return false; }
    void VulkanBuffer::WriteToBuffer(void* data, uint64_t size, uint64_t offset)
    {
        CHECKF(!_Data, "Cannot copy to unmapped buffer");
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
}