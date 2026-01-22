#pragma once
#include "VulkanContext.h"
#include "Renderer/Texture.h"

namespace House {
	static VkFormat TextureImageFormatToVulkanFormat(TextureImageFormat format) {
		switch (format)
		{
		case House::TextureImageFormat::R16F: return VK_FORMAT_R16_UNORM;
		case House::TextureImageFormat::RGBA: return VK_FORMAT_R8G8B8A8_SRGB;
		case House::TextureImageFormat::RGBA16F: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case House::TextureImageFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case House::TextureImageFormat::DEPTH32F: return VK_FORMAT_D32_SFLOAT;
		case House::TextureImageFormat::DEPTH24STENCIL8: return VK_FORMAT_D24_UNORM_S8_UINT;
		}
	}

	class VulkanTexture : public Texture2D
	{
	public:
		VulkanTexture(const TextureSpecification& spec);
		VulkanTexture(const TextureSpecification& spec, const std::string& path);
		VulkanTexture(const TextureSpecification& spec, DataBuffer data);
		~VulkanTexture();

		VkDescriptorImageInfo GetImageDescriptorInfo();
		VkFormat GetFormat() const { return _TextureFormat; }
		VkImage GetImage() const { return _TextureImage; }
		VkImageView GetImageView() const { return _TextureImageView; }
		uint32_t GetMipLevels() const { return _Specs.MipLevels; }
		virtual ImTextureID GetImGuiTextureID() override;

		void SetImage(VkImage image) { _TextureImage = image; }
		void SetImageView(VkImageView image) { _TextureImageView = image; }
	private:
		void LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels);
		void CreateTexture();
		void CreateTextureImageView();
		void CreateTextureSampler();

		VkImage _TextureImage;
		VkImageView _TextureImageView;
		VkSampler _TextureImageSampler;
		VkFormat _TextureFormat;
		VkDeviceMemory _TextureImageMemory;
		VkDescriptorSet _ImGuiDescriptorSet = VK_NULL_HANDLE;
		TextureSpecification _Specs;
		VulkanContext& _Context;
	};
}