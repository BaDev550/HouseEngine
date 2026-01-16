#pragma once
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanPipeline.h"
#include "Utilities/Memory.h"
#include <glm/glm.hpp>

class Material : public MEM::RefCounted {
private:
	struct MaterialVariables {
		MEM::Ref<VulkanTexture> DiffuseTexture = nullptr;
		MEM::Ref<VulkanTexture> NormalTexture = nullptr;
		float Metallic = 0.0f;
		float Roughness = 0.0f;

		bool HasNormalMap = false;
	} _MaterialVariables;
public:
	Material(MEM::Ref<VulkanPipeline>& pipeline);
	~Material() = default;
	Material(const Material&) = delete;
	Material& operator=(Material&) = delete;

	void Build();
	void Bind(VkCommandBuffer cmd);
	MaterialVariables& GetMaterialVariables() { return _MaterialVariables; }
	void SetMaterialVariables(const MaterialVariables& newVar) { _MaterialVariables = newVar; }
private:
	uint32_t _Id = UINT32_MAX;
	MEM::Ref<VulkanPipeline> _Pipeline;
	std::vector<VkDescriptorSet> _DescriptorSets;
	MEM::Ref<VulkanDescriptorPool> _MaterialPool;

	friend class Model;
};
