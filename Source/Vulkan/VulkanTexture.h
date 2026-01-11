#pragma once
#include "VulkanContext.h"

class VulkanTexture
{
public:
	VulkanTexture(const std::string& path);
	~VulkanTexture();
private:
	VkImage _TextureImage;
	VkDeviceMemory _TextureImageMemory;
	VulkanContext& _Context;
};