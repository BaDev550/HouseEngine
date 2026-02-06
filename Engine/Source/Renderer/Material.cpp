#include "hepch.h"
#include "Material.h"
#include "AssetManager/AssetManager.h"

#include "Vulkan/VulkanMaterial.h"

namespace House {
	static const std::string s_UniformAlbedoColor = "uMaterial.albedoColor";
	static const std::string s_UniformRoughness = "uMaterial.roughness";
	static const std::string s_UniformMetallic = "uMaterial.metallic";

	MEM::Ref<Material> Material::Create(MEM::Ref<Shader>& shader) {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return MEM::Ref<VulkanMaterial>::Create(shader);
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}

	glm::vec3& MaterialAsset::GetAlbedoColor()
	{
		return _Material->GetVector3(s_UniformAlbedoColor);
	}

	float& MaterialAsset::GetMetalness()
	{
		return _Material->GetFloat(s_UniformMetallic);
	}

	float& MaterialAsset::GetRoughness()
	{
		return _Material->GetFloat(s_UniformRoughness);
	}

	void MaterialAsset::SetAlbedoColor(const glm::vec3& color)
	{
		return _Material->Set(s_UniformAlbedoColor, 0.0f);
	}

	void MaterialAsset::SetMetalness(float value)
	{
		return _Material->Set(s_UniformMetallic, 0.0f);
	}

	void MaterialAsset::SetRoughness(float value)
	{
		return _Material->Set(s_UniformRoughness, 0.0f);
	}

	void MaterialAsset::SetAlbedoTexture(const AssetHandle& texture) {
		_TextureHandles.AlbedoTexture = texture;

		if (texture) {
			MEM::Ref<Texture2D> albedoTexture = AssetManager::GetAsset<Texture2D>(_TextureHandles.AlbedoTexture);
			_Material->Set("uDiffTexture", albedoTexture);
		}
	}

	void MaterialAsset::SetNormalTexture(const AssetHandle& texture) {
		_TextureHandles.NormalTexture = texture;

		if (texture) {
			MEM::Ref<Texture2D> normalTexture = AssetManager::GetAsset<Texture2D>(_TextureHandles.NormalTexture);
			_Material->Set("uNormTexture", normalTexture);
		}
	}

	void MaterialAsset::SetMetallicTexture(const AssetHandle& texture) {
		_TextureHandles.MetallicTexture = texture;

		if (texture) {
			MEM::Ref<Texture2D> normalTexture = AssetManager::GetAsset<Texture2D>(_TextureHandles.MetallicTexture);
			_Material->Set("uMetallicTexture", normalTexture);
		}
	}
}