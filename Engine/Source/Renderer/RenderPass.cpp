#include "hepch.h"
#include "RenderPass.h"

#include "Vulkan/VulkanRenderPass.h"

namespace House {
	MEM::Ref<RenderPass> RenderPass::Create(MEM::Ref<Pipeline>& pipeline)
	{
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanRenderPass>::Create(pipeline);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}