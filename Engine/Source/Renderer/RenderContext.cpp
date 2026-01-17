#include "hepch.h"
#include "RenderContext.h"

#include "Vulkan/VulkanContext.h"

namespace House {
	RenderContext* RenderContext::Create(GLFWwindow* window) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return new VulkanContext(window);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}