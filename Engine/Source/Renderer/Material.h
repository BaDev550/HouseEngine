#pragma once
#include "Utilities/Memory.h"
#include "Texture.h"
#include "Pipeline.h"
#include "AssetManager/Asset.h"
#include <glm/glm.hpp>

namespace House {
	struct MaterialData {
		glm::vec3 AlbedoColor = glm::vec3(1.0f);
		float Metallic = 0.0f;
		float Roughness = 1.0f;
	};

	class Material : public MEM::RefCounted {
	public:
		static MEM::Ref<Material> Create(MEM::Ref<Shader>& shader);
		virtual ~Material() = default;
		
		virtual void Build() = 0;
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
	};

	class MaterialAsset : public Asset {
	public:
		MaterialAsset(const MEM::Ref<Material>& material) : _Material(material) {}

		MEM::Ref<Material>& GetMaterial() { return _Material; }
		AssetHandle& GetAlbedoTexture() { return _TextureHandles.AlbedoTexture; }
		AssetHandle& GetNormalTexture() { return _TextureHandles.NormalTexture; }
		AssetHandle& GetMetallicTexture() { return _TextureHandles.MetallicTexture; }
		glm::vec3& GetAlbedoColor();
		float& GetMetalness();
		float& GetRoughness();

		void SetAlbedoColor(const glm::vec3& color);
		void SetMetalness(float value);
		void SetRoughness(float value);
		void SetAlbedoTexture(const AssetHandle& texture);
		void SetNormalTexture(const AssetHandle& texture);
		void SetMetallicTexture(const AssetHandle& texture);
	private:
		MEM::Ref<Material> _Material;

		struct TextureHandles {
			AssetHandle AlbedoTexture;
			AssetHandle NormalTexture;
			AssetHandle MetallicTexture;
		} _TextureHandles;
	};
}