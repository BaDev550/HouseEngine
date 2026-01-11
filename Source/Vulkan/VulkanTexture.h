#pragma once
#include "VulkanContext.h"

class VulkanTexture
{
public:
	VulkanTexture(const std::string& path);
	~VulkanTexture();
	VkDescriptorImageInfo GetImageDescriptorInfo();
private:
	void CreateTextureImageView();
	void CreateTextureSampler();

	VkImage _TextureImage;
	VkImageView _TextureImageView;
	VkSampler _TextureImageSampler;
	VkFormat _TextureFormat;
	VkDeviceMemory _TextureImageMemory;
	VulkanContext& _Context;
};