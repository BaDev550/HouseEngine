#pragma once
#include "Utilities/Memory.h"
#include "Texture.h"
#include "Pipeline.h"
#include <glm/glm.hpp>

namespace House {
	struct MaterialData {
		glm::vec4 AlbedoColor = glm::vec4(1.0f);
		float Metallic = 0.0f;
		float Roughness = 1.0f;
		MEM::Ref<Texture2D> AlbedoTexture;
		MEM::Ref<Texture2D> NormalTexture;
	};

	class Material : public MEM::RefCounted {
	private:
	public:
		virtual ~Material() = default;
		
		virtual void Build() = 0;
		virtual void Bind() = 0;

		MaterialData& GetMaterialData() { return _Data; };
		void SetMaterialData(const MaterialData& data) { _Data = data; }

		static MEM::Ref<Material> Create(MEM::Ref<Pipeline>& pipeline);
	protected:
		uint32_t _Id = UINT32_MAX;
		MaterialData _Data;

		friend class Model;
	};
}