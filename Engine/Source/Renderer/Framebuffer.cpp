#include "hepch.h"
#include "Framebuffer.h"

#include "Vulkan/VulkanFramebuffer.h"

namespace House {
	MEM::Ref<Framebuffer> Create(const FramebufferSpecification& spec) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanFramebuffer>::Create(spec);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}
