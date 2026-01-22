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
	};

	class Material : public MEM::RefCounted {
	private:
	public:
		virtual ~Material() = default;
		
		virtual void Build() = 0;
		virtual void Bind() = 0;

		uint32_t GetID() const { return _Id; }

		glm::vec4& GetAlbedoColor() { return _Data.AlbedoColor; }
		void SetAlbedoColor(const glm::vec4& color) { _Data.AlbedoColor = color; MaterialDataChanged(); }

		float GetMetallic() const { return _Data.Metallic; }
		void SetMetallic(float metallic) { _Data.Metallic = metallic; MaterialDataChanged(); }

		float GetRoughness() const { return _Data.Roughness; }
		void SetRoughness(float roughness) { _Data.Roughness = roughness; MaterialDataChanged(); }

		MEM::Ref<Texture2D>& GetAlbedoTexture() { return _AlbedoTexture; }
		void SetAlbedoTexture(const MEM::Ref<Texture2D>& texture) { _AlbedoTexture = texture; }

		MEM::Ref<Texture2D>& GetNormalTexture() { return _NormalTexture; }
		void SetNormalTexture(const MEM::Ref<Texture2D>& texture) { _NormalTexture = texture; }

		MEM::Ref<Texture2D>& GetMetallicRoughnessTexture() { return _MetallicRoughnessTexture; }
		void SetMetallicRoughnessTexture(const MEM::Ref<Texture2D>& texture) { _MetallicRoughnessTexture = texture; }

		static MEM::Ref<Material> Create(MEM::Ref<Pipeline>& pipeline);
	protected:
		virtual void MaterialDataChanged() = 0;

		uint32_t _Id = UINT32_MAX;
		MaterialData _Data;
		MEM::Ref<Texture2D> _AlbedoTexture;
		MEM::Ref<Texture2D> _NormalTexture;
		MEM::Ref<Texture2D> _MetallicRoughnessTexture;

		friend class Model;
	};
}