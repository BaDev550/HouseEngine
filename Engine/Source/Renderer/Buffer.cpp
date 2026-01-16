#include "hepch.h"
#include "Buffer.h"

#include "Vulkan/VulkanBuffer.h"

namespace House {
	MEM::Ref<Buffer> Buffer::Create(uint64_t size, BufferFlags typeFlags, MemoryFlags memprops) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanBuffer>::Create(size, typeFlags, memprops);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}

#if 0
	MEM::Ref<Buffer> UniformBuffer::Create(uint64_t size) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanBuffer>::Create(size, BufferType::UniformBuffer, MemoryProperties::HOST_COHERENT);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
#endif
}