#pragma once
#include "VulkanContext.h"
#include "Renderer/Texture.h"

class VulkanTexture : public Texture2D
{
public:
	VulkanTexture(const std::string& path);
	VulkanTexture(uint32_t* data, uint32_t width, uint32_t height);
	~VulkanTexture();
	VkDescriptorImageInfo GetImageDescriptorInfo();
private:
	void LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels);
	void CreateTextureImageView();
	void CreateTextureSampler();

	VkImage _TextureImage;
	VkImageView _TextureImageView;
	VkSampler _TextureImageSampler;
	VkFormat _TextureFormat;
	VkDeviceMemory _TextureImageMemory;
	VulkanContext& _Context;
};