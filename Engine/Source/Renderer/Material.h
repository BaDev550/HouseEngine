#pragma once
#include "Utilities/Memory.h"
#include "Texture.h"
#include "Pipeline.h"
#include <glm/glm.hpp>

namespace House {
	struct MaterialData {
		glm::vec3 AlbedoColor = glm::vec3(1.0f);
		float Metallic = 0.0f;
		float Roughness = 1.0f;
	};

	class Material : public MEM::RefCounted {
	public:
		static MEM::Ref<Material> Create(MEM::Ref<Pipeline>& pipeline);
		virtual ~Material() = default;
		
		virtual void Build() = 0;
		virtual void Bind() = 0;

		virtual void Set(const std::string& name, float value) = 0;
		virtual void Set(const std::string& name, int value) = 0;
		virtual void Set(const std::string& name, bool value) = 0;
		virtual void Set(const std::string& name, const glm::vec2& value) = 0;
		virtual void Set(const std::string& name, const glm::vec3& value) = 0;
		virtual void Set(const std::string& name, const glm::vec4& value) = 0;
		virtual void Set(const std::string& name, const MEM::Ref<Texture2D>& value) = 0;

		virtual float& GetFloat(const std::string& name) = 0;
		virtual glm::vec2& GetVector2(const std::string& name) = 0;
		virtual glm::vec3& GetVector3(const std::string& name) = 0;

		virtual void MaterialDataChanged() = 0;
	protected:
		friend class Model;
	};

	class MaterialAsset : public Asset {

	};
#if 0
	AssetHandle _AlbedoTextureHandle;
	AssetHandle _NormalTextureHandle;
	AssetHandle _MetallicRoughnessTextureHandle;

	uint32_t GetID() const { return _Id; }

	glm::vec3& GetAlbedoColor() { return _Data.AlbedoColor; }
	void SetAlbedoColor(const glm::vec3& color) { _Data.AlbedoColor = color; MaterialDataChanged(); }

	float GetMetallic() const { return _Data.Metallic; }
	void SetMetallic(float metallic) { _Data.Metallic = metallic; MaterialDataChanged(); }

	float GetRoughness() const { return _Data.Roughness; }
	void SetRoughness(float roughness) { _Data.Roughness = roughness; MaterialDataChanged(); }

	AssetHandle& GetAlbedoTexture() { return _AlbedoTextureHandle; }
	void SetAlbedoTexture(const AssetHandle& texture) { _AlbedoTextureHandle = texture; }

	AssetHandle& GetNormalTexture() { return _NormalTextureHandle; }
	void SetNormalTexture(const AssetHandle& texture) { _NormalTextureHandle = texture; }

	AssetHandle& GetMetallicRoughnessTexture() { return _MetallicRoughnessTextureHandle; }
	void SetMetallicRoughnessTexture(const AssetHandle& texture) { _MetallicRoughnessTextureHandle = texture; }

#endif
}