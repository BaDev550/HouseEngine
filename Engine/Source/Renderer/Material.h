#pragma once
#include "Utilities/Memory.h"
#include "Texture.h"
#include "Pipeline.h"
#include <glm/glm.hpp>

namespace House {
	struct MaterialData {
		MEM::Ref<Texture2D> DiffuseTexture = nullptr;
		MEM::Ref<Texture2D> NormalTexture = nullptr;
		float Metallic = 0.0f;
		float Roughness = 0.0f;

		bool HasNormalMap = false;
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