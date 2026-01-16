#pragma once
#include "Utilities/Memory.h"
#include "Texture.h"
#include "Pipeline.h"
#include <glm/glm.hpp>

namespace House {
	class Material : public MEM::RefCounted {
	private:
		struct MaterialVariables {
			MEM::Ref<Texture2D> DiffuseTexture = nullptr;
			MEM::Ref<Texture2D> NormalTexture = nullptr;
			float Metallic = 0.0f;
			float Roughness = 0.0f;

			bool HasNormalMap = false;
		} _MaterialVariables;
	public:
		Material(MEM::Ref<Pipeline>& pipeline);
		~Material() = default;
		Material(const Material&) = delete;
		Material& operator=(Material&) = delete;

		void Build();
		void Bind(VkCommandBuffer cmd);
		MaterialVariables& GetMaterialVariables() { return _MaterialVariables; }
		void SetMaterialVariables(const MaterialVariables& newVar) { _MaterialVariables = newVar; }
	private:
		uint32_t _Id = UINT32_MAX;
		MEM::Ref<Pipeline> _Pipeline;
		//std::vector<VkDescriptorSet> _DescriptorSets;
		//MEM::Ref<VulkanDescriptorPool> _MaterialPool;

		friend class Model;
	};
}