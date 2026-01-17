#include "ImGuiLayer.h"

#include "Vulkan/VulkanImGuiLayer.h"

namespace House {
	ImGuiLayer* ImGuiLayer::Create()
	{
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return new VulkanImGuiLayer();
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}