#include "hepch.h"
#include "VulkanTexture.h"
#include "Core/Application.h"

#include <stb_image.h>

namespace House {
	namespace Utils {
		VkImageAspectFlags TextureImageFormatToVulkanAspectFormat(TextureImageFormat format) {
			switch (format)
			{
			case House::TextureImageFormat::None:
				break;
			case House::TextureImageFormat::RG16F:	 return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::RG32F:	 return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::RGB:	 return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::RGBA:	 return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::RGBA16F: return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::RGBA32F: return VK_IMAGE_ASPECT_COLOR_BIT;
			case House::TextureImageFormat::DEPTH32F:return VK_IMAGE_ASPECT_DEPTH_BIT;
			case House::TextureImageFormat::DEPTH24STENCIL8: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
	}
	VulkanTexture::VulkanTexture(const TextureSpecification& spec)
		: _Context(Application::Get()->GetVulkanContext())
	{
		_Specs = spec;
		CreateTexture();
	}
	VulkanTexture::VulkanTexture(const TextureSpecification& spec, const std::string& path)
		: _Context(Application::Get()->GetVulkanContext())
	{
		_Specs = spec;
		int width, height, channels;
		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels) {
			LOG_CORE_WARN("Failed to load {} using fallback white texture", path);
			return;
		}
		_Specs.Width = width;
		_Specs.Height = height;
		LoadTexture(pixels, width, height, STBI_rgb_alpha);
		stbi_image_free(pixels);
	}

	VulkanTexture::VulkanTexture(const TextureSpecification& spec, DataBuffer data)
		: _Context(Application::Get()->GetVulkanContext())
	{
		_Specs = spec;
		LoadTexture(data.Data, spec.Width, spec.Height, STBI_rgb_alpha);
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

	void VulkanTexture::LoadTexture(void* data, uint32_t width, uint32_t height, uint32_t channels)
	{
		uint64_t imageSize = width * height * STBI_rgb_alpha;
		if (data) {
			MEM::Scope<VulkanBuffer> stagingBuffer;
			stagingBuffer = MEM::MakeScope<VulkanBuffer>(
				imageSize,
				BufferType::TransferSrc,
				MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT
			);
			stagingBuffer->Map();
			stagingBuffer->WriteToBuffer(data);
			stagingBuffer->Unmap();
			CreateTexture();
			_Context.TransitionImageLayout(_TextureImage, _TextureFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			_Context.CopyBufferToImage(stagingBuffer->GetBuffer(), _TextureImage, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
			_Context.TransitionImageLayout(_TextureImage, _TextureFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		else {
			LOG_RENDERER_ERROR("Failed to laod texture");
		}
	}

	void VulkanTexture::CreateTexture()
	{
		VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		if (_Specs.Attachment) {
			if (IsDepthFormat(_Specs.Format)) {
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else {
				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
		}
		else {
			usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		_TextureFormat = TextureImageFormatToVulkanFormat(_Specs.Format);
		_Context.CreateImage(
			_Specs.Width,
			_Specs.Height,
			_TextureFormat,
			VK_IMAGE_TILING_OPTIMAL,
			usage,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _TextureImage, _TextureImageMemory);
		CreateTextureImageView();
		CreateTextureSampler();
	}

	void VulkanTexture::CreateTextureImageView()
	{
		VkImageViewCreateInfo viewCreateInfo{};
		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.image = _TextureImage;
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = _TextureFormat;
		viewCreateInfo.subresourceRange.aspectMask = Utils::TextureImageFormatToVulkanAspectFormat(_Specs.Format);
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
}