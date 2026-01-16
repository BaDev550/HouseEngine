#include "hepch.h"
#include "Pipeline.h"

#include "Vulkan/VulkanPipeline.h"

namespace House {
	MEM::Ref<Pipeline> Pipeline::Create(const PipelineData& data)
	{
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanPipeline>::Create(data);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}