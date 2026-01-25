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

		virtual void Set(const std::string& name, float value) override;
		virtual void Set(const std::string& name, int value) override;
		virtual void Set(const std::string& name, bool value) override;
		virtual void Set(const std::string& name, const glm::vec2& value) override;
		virtual void Set(const std::string& name, const glm::vec3& value) override;
		virtual void Set(const std::string& name, const glm::vec4& value) override;
		virtual void Set(const std::string& name, const MEM::Ref<Texture2D>& value) override;

		virtual float& GetFloat(const std::string& name) override;
		virtual glm::vec2& GetVector2(const std::string& name) override;
		virtual glm::vec3& GetVector3(const std::string& name) override;

		virtual void MaterialDataChanged() override;
	private:
		MaterialData _Data;
		DescriptorManager _DescriptorManager;
		std::vector<VkDescriptorSet> _DescriptorSets;
		MEM::Ref<VulkanBuffer> _MaterialBuffer;
		MEM::Ref<VulkanPipeline> _Pipeline;

		friend class Model;
	};
}