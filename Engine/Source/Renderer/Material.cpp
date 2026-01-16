#include "hepch.h"
#include "Material.h"

#include "Vulkan/VulkanMaterial.h"

namespace House {
	MEM::Ref<Material> Material::Create(MEM::Ref<Pipeline>& pipeline) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanMaterial>::Create(pipeline);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}
}