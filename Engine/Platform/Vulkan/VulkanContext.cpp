#include "hepch.h"
#include "VulkanContext.h"
#include <iostream>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <set>
#include "Core/Application.h"
#include "Core/Window.h"

namespace House {
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT              messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			LOG_RENDERER_ERROR("Validation layer : {}", pCallbackData->pMessage);
		return VK_FALSE;
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance                                  instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(
		VkInstance                                  instance,
		VkDebugUtilsMessengerEXT                    debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanContext::DefaultPipelineConfigInfo(VulkanPipelineConfig& config)
	{
		config.InputAssembyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config.InputAssembyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config.InputAssembyCreateInfo.primitiveRestartEnable = VK_FALSE;

		config.ViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config.ViewportStateCreateInfo.viewportCount = 1;
		config.ViewportStateCreateInfo.scissorCount = 1;
		config.ViewportStateCreateInfo.pViewports = nullptr;
		config.ViewportStateCreateInfo.pScissors = nullptr;

		config.ResterizationStateCreateInfo = {};
		config.ResterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config.ResterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		config.ResterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		config.ResterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		config.ResterizationStateCreateInfo.lineWidth = 1.0f;
		config.ResterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		config.ResterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		config.ResterizationStateCreateInfo.depthBiasClamp = 0.0f;
		config.ResterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

		config.MultisampleStateCreateInfo = {};
		config.MultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config.MultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		config.MultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		config.MultisampleStateCreateInfo.minSampleShading = 1.0f;
		config.MultisampleStateCreateInfo.pSampleMask = nullptr;
		config.MultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		config.MultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		config.ColorBlendAttachment = {};
		config.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config.ColorBlendAttachment.blendEnable = VK_FALSE;
		config.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		config.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		config.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		config.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		config.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		config.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		config.ColorBlendStateCreateInfo = {};
		config.ColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config.ColorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		config.ColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		config.ColorBlendStateCreateInfo.attachmentCount = 1;
		config.ColorBlendStateCreateInfo.pAttachments = &config.ColorBlendAttachment;
		config.ColorBlendStateCreateInfo.blendConstants[0] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[1] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[2] = 0.0f;
		config.ColorBlendStateCreateInfo.blendConstants[3] = 0.0f;

		config.DepthStencilCreateInfo = {};
		config.DepthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config.DepthStencilCreateInfo.depthTestEnable = VK_TRUE;
		config.DepthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		config.DepthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		config.DepthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		config.DepthStencilCreateInfo.minDepthBounds = 0.0f;
		config.DepthStencilCreateInfo.maxDepthBounds = 1.0f;
		config.DepthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		config.DepthStencilCreateInfo.front = {};
		config.DepthStencilCreateInfo.back = {};

		config.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config.DynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config.DynamicStateCreateInfo.pDynamicStates = config.DynamicStateEnables.data();
		config.DynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(config.DynamicStateEnables.size());
		config.DynamicStateCreateInfo.flags = 0;
	}

	VulkanContext::VulkanContext(GLFWwindow* window)
	{
		LOG_RENDERER_INFO("Using Vulkan grapichs API");
		try {
			CreateInstance();
			SetupDebugMessenger();
			CreateSurface(window);
			PickPhysicalDevice();
			CreateLogicalDevice();
			CreateContextCommandPool();
		}
		catch (const std::exception& e) {
			LOG_RENDERER_ERROR("VulkanContext initialization failed: {}", e.what());
			throw;
		}
	}

	VulkanContext::~VulkanContext()
	{
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(_Instance, _DebugMessenger, nullptr);

		if (_CommandPool != VK_NULL_HANDLE) vkDestroyCommandPool(_Device, _CommandPool, nullptr);
		if (_Surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(_Instance, _Surface, nullptr);
		if (_Device != VK_NULL_HANDLE) vkDestroyDevice(_Device, nullptr);
		if (_Instance != VK_NULL_HANDLE) vkDestroyInstance(_Instance, nullptr);
		LOG_RENDERER_INFO("VulkanContext destroyed successfully.");
	}

	void VulkanContext::CreateInstance()
	{
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = Application::Get()->GetApplicationSpecs().Name.c_str();
		appInfo.pEngineName = "HouseEngine";
		appInfo.apiVersion = VK_API_VERSION_1_3;

		CHECKF((enableValidationLayers && !CheckValidationLayerSupport()), "Validation layers requested, but not available!");

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = _ValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		CHECKF((vkCreateInstance(&createInfo, nullptr, &_Instance) != VK_SUCCESS), "Failed to create Vulkan instance")
	}

	void VulkanContext::PickPhysicalDevice()
	{
		uint32_t physicalDeviceCount = 0;
		vkEnumeratePhysicalDevices(_Instance, &physicalDeviceCount, nullptr);

		CHECKF((physicalDeviceCount == 0), "Failed to find GPUs with Vulkan support");
		std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
		vkEnumeratePhysicalDevices(_Instance, &physicalDeviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsPhysicalDeviceSuitable(device)) {
				_PhysicalDevice = device;
				break;
			}
		}

		CHECKF(_PhysicalDevice == VK_NULL_HANDLE, "Failed to find a suitable GPU")

			vkGetPhysicalDeviceProperties(_PhysicalDevice, &_PhysicalDeviceProperties);
		LOG_RENDERER_INFO("Selected GPU Properties: ");
		LOG_RENDERER_INFO("	GPU Name: {}", _PhysicalDeviceProperties.deviceName);
		LOG_RENDERER_INFO("	GPU Driver version: {}", _PhysicalDeviceProperties.driverVersion);
	}

	void VulkanContext::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(_PhysicalDevice);
		float queuePriority = 1.0f;

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
		dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
		dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
		descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
		descriptorIndexingFeatures.pNext = &dynamicRenderingFeatures;
		descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
		descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
		descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
		descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = &descriptorIndexingFeatures;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = _DeviceExtensions.data();
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = _ValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}
		CHECKF(vkCreateDevice(_PhysicalDevice, &createInfo, nullptr, &_Device) != VK_SUCCESS, "Failed to create logical device");

		vkGetDeviceQueue(_Device, indices.graphicsFamily.value(), 0, &_GraphicsQueue);
		vkGetDeviceQueue(_Device, indices.presentFamily.value(), 0, &_PresentQueue);
	}

	void VulkanContext::CreateSurface(GLFWwindow* window)
	{
		CHECKF(glfwCreateWindowSurface(_Instance, window, nullptr, &_Surface) != VK_SUCCESS, "Failed to create window surface!");
	}

	void VulkanContext::CreateContextCommandPool()
	{
		QueueFamilyIndices indices = FindQueueFamilies(_PhysicalDevice);
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = indices.graphicsFamily.value();
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		CHECKF(vkCreateCommandPool(_Device, &createInfo, nullptr, &_CommandPool) != VK_SUCCESS, "Failed to create context command pool");
	}

	uint32_t VulkanContext::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_PhysicalDevice, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		CHECKF(false, "Failed to find any usable memory type");
	}

	VkFormat VulkanContext::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_PhysicalDevice, format, &props);
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (
				tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}
		CHECKF(false, "Failed to find supported format");
	}

	void VulkanContext::WaitDeviceIdle()
	{
		vkDeviceWaitIdle(_Device);
	}

	void VulkanContext::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.usage = usage;
		bufferInfo.size = size;

		CHECKF(vkCreateBuffer(_Device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS, "Failed to create buffer");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_Device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		CHECKF(vkAllocateMemory(_Device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS, "Failed to allocate memory for vertex buffer");
		vkBindBufferMemory(_Device, buffer, bufferMemory, 0);
	}

	void VulkanContext::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags memoryflags, VkImage& image, VkDeviceMemory& memory)
	{
		VkImageCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.extent.width = static_cast<uint32_t>(width);
		createInfo.extent.height = static_cast<uint32_t>(height);
		createInfo.extent.depth = 1;
		createInfo.mipLevels = mipLevels;
		createInfo.arrayLayers = 1;
		createInfo.format = format;
		createInfo.tiling = tiling;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.flags = 0;
		CHECKF(vkCreateImage(_Device, &createInfo, nullptr, &image) != VK_SUCCESS, "Failed to create texture image");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_Device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryflags);
		CHECKF(vkAllocateMemory(_Device, &allocInfo, nullptr, &memory) != VK_SUCCESS, "Failed allocation for image memory");
		vkBindImageMemory(_Device, image, memory, 0);
	}

	void VulkanContext::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer cmd = BeginSingleTimeCommands();
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(cmd, srcBuffer, dstBuffer, 1, &copyRegion);
		EndSingleTimeCommands(cmd);
	}

	void VulkanContext::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer cmd = BeginSingleTimeCommands();
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };
		vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		EndSingleTimeCommands(cmd);
	}

	void VulkanContext::TransitionImageLayout(VkImage image, uint32_t mipLevels, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer cmd = BeginSingleTimeCommands();
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			CHECKF(false, "Unsupported layout transition");
		}

		vkCmdPipelineBarrier(
			cmd,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		EndSingleTimeCommands(cmd);
	}

	void VulkanContext::GenerateMipmaps(VkImage image, VkFormat imageFormat, uint32_t width, uint32_t height, uint32_t mipLevels)
	{
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(_PhysicalDevice, imageFormat, &formatProperties);
		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			LOG_RENDERER_ERROR("Texture image format does not support linear blitting!");
			return;
		}

		VkCommandBuffer cmd = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mipWidth = width;
		int32_t mipHeight = height;

		for (uint32_t i = 1; i < mipLevels; i++) {
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr, 0, nullptr, 1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
			blit.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i - 1, 0, 1 };

			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, i, 0, 1 };

			vkCmdBlitImage(cmd,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr, 0, nullptr, 1, &barrier);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr, 0, nullptr, 1, &barrier);

		EndSingleTimeCommands(cmd);
	}

	void VulkanContext::SetupDebugMessenger()
	{
		if (!enableValidationLayers) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		CHECKF(CreateDebugUtilsMessengerEXT(_Instance, &createInfo, nullptr, &_DebugMessenger) != VK_SUCCESS, "Failed to set up debug messenger!");
	}

	bool VulkanContext::IsPhysicalDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		QueueFamilyIndices indices = FindQueueFamilies(device);
		bool extensionsSupported = HasRequiredDeviceExtensions(device);
		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}
		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& deviceFeatures.samplerAnisotropy
			&& indices.IsComplete()
			&& extensionsSupported
			&& swapChainAdequate;
	}

	bool VulkanContext::HasRequiredDeviceExtensions(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		std::set<std::string> requiredExtensions(_DeviceExtensions.begin(), _DeviceExtensions.end());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);
		return requiredExtensions.empty();
	}

	bool VulkanContext::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _ValidationLayers) {
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}
			if (!layerFound) {
				return false;
			}
		}
		return true;
	}

	VkCommandBuffer VulkanContext::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_Device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void VulkanContext::EndSingleTimeCommands(VkCommandBuffer cmd)
	{
		vkEndCommandBuffer(cmd);
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		vkQueueSubmit(_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_GraphicsQueue);
		vkFreeCommandBuffers(_Device, _CommandPool, 1, &cmd);
	}

	std::vector<const char*> VulkanContext::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	QueueFamilyIndices VulkanContext::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		for (int i = 0; i < queueFamilies.size(); i++) {
			if (indices.IsComplete())
				break;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _Surface, &presentSupport);

			const auto& queueFamily = queueFamilies[i];
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;
			if (presentSupport)
				indices.presentFamily = i;
		}
		return indices;
	}

	SwapChainSupportDetails VulkanContext::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _Surface, &details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _Surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _Surface, &formatCount, details.formats.data());
		}
		else {
			LOG_RENDERER_ERROR("Device has no supported surface formats.");
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _Surface, &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _Surface, &presentModeCount, details.presentModes.data());
		}
		return details;
	}
}