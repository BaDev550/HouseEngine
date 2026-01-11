#include "VulkanTexture.h"
#include "Application.h"

#include <stb/stb_image.h>

VulkanTexture::VulkanTexture(const std::string& path)
	: _Context(*Application::Get()->GetVulkanContext())
{
	int width, height, channels;
	stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	VkDeviceSize imageSize = width * height * STBI_rgb_alpha;
	if (pixels) {
		std::unique_ptr<VulkanBuffer> stagingBuffer = std::make_unique<VulkanBuffer>(
			imageSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		stagingBuffer->Map();
		stagingBuffer->WriteToBuffer(pixels);
		stagingBuffer->Unmap();
		stbi_image_free(pixels);
		_Context.CreateImage(
			width, 
			height, 
			VK_FORMAT_R8G8B8A8_SRGB, 
			VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _TextureImage, _TextureImageMemory);
		_Context.TransitionImageLayout(_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		_Context.CopyBufferToImage(stagingBuffer->GetBuffer(), _TextureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		_Context.TransitionImageLayout(_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}

VulkanTexture::~VulkanTexture()
{
	vkDestroyImage(_Context.GetDevice(), _TextureImage, nullptr);
	vkFreeMemory(_Context.GetDevice(), _TextureImageMemory, nullptr);
}
