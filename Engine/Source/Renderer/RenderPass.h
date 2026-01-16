#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Utilities/Memory.h"

#include "World/Entity/Entity.h"
#include "World/Scene/Scene.h"
#include "Camera.h"
#include <array>

namespace House {
	class RenderPass : public MEM::RefCounted
	{
	public:
		virtual ~RenderPass() = default;
		
		
	};
}