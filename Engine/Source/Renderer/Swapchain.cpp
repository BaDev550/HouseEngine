#include "hepch.h"
#include "Swapchain.h"

#include "Vulkan/VulkanSwapchain.h"

namespace House {
	Swapchain* Swapchain::Create(RenderContext* Context) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return new VulkanSwapchain(dynamic_cast<VulkanContext*>(Context));
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}