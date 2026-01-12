#pragma once
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanPipeline.h"
#include "Core/Memory.h"
#include <glm/glm.hpp>

class Material {
private:
	struct MaterialVariables {
		VkDescriptorSet _MaterialDescriptorSet;

		MEM::Ref<VulkanTexture> _DiffuseTexture = nullptr;
		MEM::Ref<VulkanTexture> _NormalTexture = nullptr;
		float Metallic = 0.0f;
		float Roughness = 0.0f;
	} _MaterialVariables;
public:
	Material(VulkanPipeline& pipeline) : _OwnerPipeline(pipeline) {}
	~Material() {}
	Material(const Material&) = delete;
	Material& operator=(Material&) = delete;
	MaterialVariables& GetMaterialVariables() { return _MaterialVariables; }
	void SetMaterialVariables(const MaterialVariables& newVar) { _MaterialVariables = newVar; }
private:
	uint32_t _Id = UINT32_MAX;
	VulkanPipeline& _OwnerPipeline;

	friend class Model;
};
