#pragma once
#include "VulkanContext.h"
#include "Renderer/Texture.h"

namespace House {
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
	private:
		void LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels);
		void CreateTextureImageView();
		void CreateTextureSampler();

		VkImage _TextureImage;
		VkImageView _TextureImageView;
		VkSampler _TextureImageSampler;
		VkFormat _TextureFormat;
		VkDeviceMemory _TextureImageMemory;
		TextureSpecification _Specs;
		VulkanContext& _Context;
	};
}