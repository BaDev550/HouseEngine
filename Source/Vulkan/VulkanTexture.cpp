#include "hepch.h"
#include "VulkanTexture.h"
#include "Core/Application.h"

#include <stb_image.h>

VulkanTexture::VulkanTexture(const std::string& path)
	: _Context(*Application::Get()->GetVulkanContext())
{
	int width, height, channels;
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	VkDeviceSize imageSize = width * height * STBI_rgb_alpha;
	CHECKF(!pixels, "Failed to load texture");

	std::unique_ptr<VulkanBuffer> stagingBuffer;
	stagingBuffer = std::make_unique<VulkanBuffer>(
		imageSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer->Map();
	stagingBuffer->WriteToBuffer(pixels);
	stagingBuffer->Unmap();
	stbi_image_free(pixels);
	_TextureFormat = VK_FORMAT_R8G8B8A8_SRGB;
	_Context.CreateImage(
		width,
		height,
		_TextureFormat,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _TextureImage, _TextureImageMemory);
	_Context.TransitionImageLayout(_TextureImage, _TextureFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	_Context.CopyBufferToImage(stagingBuffer->GetBuffer(), _TextureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	_Context.TransitionImageLayout(_TextureImage, _TextureFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	CreateTextureImageView();
	CreateTextureSampler();
}

VulkanTexture::~VulkanTexture()
{
	vkDestroySampler(_Context.GetDevice(), _TextureImageSampler, nullptr);
	vkDestroyImageView(_Context.GetDevice(), _TextureImageView, nullptr);
	vkDestroyImage(_Context.GetDevice(), _TextureImage, nullptr);
	vkFreeMemory(_Context.GetDevice(), _TextureImageMemory, nullptr);
}

VkDescriptorImageInfo VulkanTexture::GetImageDescriptorInfo()
{
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = _TextureImageView;
	imageInfo.sampler = _TextureImageSampler;
	return imageInfo;
}

void VulkanTexture::CreateTextureImageView()
{
	VkImageViewCreateInfo viewCreateInfo{};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = _TextureImage;
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.format = _TextureFormat;
	viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewCreateInfo.subresourceRange.baseMipLevel = 0;
	viewCreateInfo.subresourceRange.levelCount = 1;
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;
	viewCreateInfo.subresourceRange.layerCount = 1;
	CHECKF((vkCreateImageView(_Context.GetDevice(), &viewCreateInfo, nullptr, &_TextureImageView) != VK_SUCCESS), "Failed to create Image view");
}

void VulkanTexture::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerCreateInfo{};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = _Context.GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	CHECKF((vkCreateSampler(_Context.GetDevice(), &samplerCreateInfo, nullptr, &_TextureImageSampler) != VK_SUCCESS), "Failed to create texture sampler");
}
