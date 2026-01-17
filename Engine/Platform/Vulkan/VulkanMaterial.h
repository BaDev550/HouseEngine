#pragma once
#include "Utilities/Memory.h"
#include "Renderer/Material.h"
#include "VulkanDescriptor.h"
#include "VulkanPipeline.h"
#include "VulkanDescriptorManager.h"
#include <glm/glm.hpp>

namespace House {
	class VulkanMaterial : public Material {
	public:
		VulkanMaterial(MEM::Ref<Pipeline>& pipeline);
		~VulkanMaterial() = default;
		VulkanMaterial(const VulkanMaterial&) = delete;
		VulkanMaterial& operator=(VulkanMaterial&) = delete;

		virtual void Build() override;
		virtual void Bind() override;
	private:
		DescriptorManager _DescriptorManager;
		std::vector<VkDescriptorSet> _DescriptorSets;
		MEM::Ref<VulkanPipeline> _Pipeline;

		friend class Model;
	};
}