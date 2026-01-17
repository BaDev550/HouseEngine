#include "hepch.h"
#include "VulkanSwapchain.h"
#include <iostream>
#include <algorithm>
#include "Core/Application.h"

namespace House {
	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				LOG_RENDERER_INFO("Using MAILBOX present mode.");
				return availablePresentMode;
			}
		}
		LOG_RENDERER_INFO("Using VSYNC present mode.");
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			auto& window = Application::Get()->GetWindow();
			width = window.GetWidth();
			height = window.GetHeight();

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	VkFormat VulkanSwapchain::FindDepthFormat()
	{
		return _Context.FindSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VulkanSwapchain::VulkanSwapchain(VulkanContext* context)
		: _Context(*context)
	{
		try {
			CreateSwapChain();
			CreateDepthResources();
			CreateRenderPass();
			CreateImageViews();
			CreateFramebuffers();
			CreateSyncObjects();
		}
		catch (const std::exception& e) {
			LOG_RENDERER_ERROR("SwapChain: {}", e.what());
		}
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		DestroySwapchain();
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (_ImageAvailableSemaphores[i] != VK_NULL_HANDLE) vkDestroySemaphore(_Context.GetDevice(), _ImageAvailableSemaphores[i], nullptr);
			if (_InFlightFences[i] != VK_NULL_HANDLE) vkDestroyFence(_Context.GetDevice(), _InFlightFences[i], nullptr);
		}

		for (size_t i = 0; i < GetImageCount(); i++) {
			if (_RenderFinishedSemaphores[i] != VK_NULL_HANDLE) vkDestroySemaphore(_Context.GetDevice(), _RenderFinishedSemaphores[i], nullptr);
		}

		_ImageAvailableSemaphores.clear();
		_RenderFinishedSemaphores.clear();
		_InFlightFences.clear();
		_SwapChainFramebuffers.clear();
		_SwapChainImageViews.clear();

		if (_RenderPass != VK_NULL_HANDLE) vkDestroyRenderPass(_Context.GetDevice(), _RenderPass, nullptr);
	}

	void VulkanSwapchain::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = _Context.QuerySwapChainSupportOnPhysicalDevice();
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _Context.GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		CHECKF(vkCreateSwapchainKHR(_Context.GetDevice(), &createInfo, nullptr, &_SwapChain) != VK_SUCCESS, "Failed to create swap chain!");

		vkGetSwapchainImagesKHR(_Context.GetDevice(), _SwapChain, &imageCount, nullptr);
		_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_Context.GetDevice(), _SwapChain, &imageCount, _SwapChainImages.data());

		_SwapChainExtent = extent;
		_SwapChainImageFormat = surfaceFormat.format;
	}

	void VulkanSwapchain::CreateDepthResources()
	{
		VkFormat depthFormat = FindDepthFormat();
		_Context.CreateImage(
			_SwapChainExtent.width,
			_SwapChainExtent.height,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			_DepthImage,
			_DepthImageMemory
		);
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = _DepthImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		CHECKF(vkCreateImageView(_Context.GetDevice(), &viewInfo, nullptr, &_DepthImageView) != VK_SUCCESS, "Failed to create depth image view");
	}

	void VulkanSwapchain::CreateImageViews() {
		_SwapChainImageViews.resize(_SwapChainImages.size());
		for (size_t i = 0; i < _SwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = _SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = _SwapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			CHECKF(vkCreateImageView(_Context.GetDevice(), &createInfo, nullptr, &_SwapChainImageViews[i]) != VK_SUCCESS, "Failed to create image views!");
		}
	}

	void VulkanSwapchain::CreateRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = _SwapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		CHECKF(vkCreateRenderPass(_Context.GetDevice(), &createInfo, nullptr, &_RenderPass) != VK_SUCCESS, "Failed to create render pass");
	}

	void VulkanSwapchain::CreateFramebuffers() {
		_SwapChainFramebuffers.resize(_SwapChainImages.size());
		for (size_t i = 0; i < _SwapChainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachments =
			{
				_SwapChainImageViews[i],
				_DepthImageView
			};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = _RenderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = _SwapChainExtent.width;
			framebufferCreateInfo.height = _SwapChainExtent.height;
			framebufferCreateInfo.layers = 1;

			CHECKF(vkCreateFramebuffer(_Context.GetDevice(), &framebufferCreateInfo, nullptr, &_SwapChainFramebuffers[i]) != VK_SUCCESS, "Failed to create swapchain framebuffers");
		}
	}

	void VulkanSwapchain::CreateSyncObjects() {
		_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_RenderFinishedSemaphores.resize(GetImageCount());
		_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			CHECKF(vkCreateSemaphore(_Context.GetDevice(), &semaphoreCreateInfo, nullptr, &_ImageAvailableSemaphores[i]) != VK_SUCCESS
				|| vkCreateFence(_Context.GetDevice(), &fenceCreateInfo, nullptr, &_InFlightFences[i]) != VK_SUCCESS,
				"Failed to create syncned objects");
		}

		for (size_t i = 0; i < GetImageCount(); i++) {
			CHECKF(vkCreateSemaphore(_Context.GetDevice(), &semaphoreCreateInfo, nullptr, &_RenderFinishedSemaphores[i]) != VK_SUCCESS, "Failed to create syncned objects");
		}
	}

	void VulkanSwapchain::DestroySwapchain() {
		vkDestroyImageView(_Context.GetDevice(), _DepthImageView, nullptr);
		vkDestroyImage(_Context.GetDevice(), _DepthImage, nullptr);
		vkFreeMemory(_Context.GetDevice(), _DepthImageMemory, nullptr);

		for (auto framebuffer : _SwapChainFramebuffers)
			vkDestroyFramebuffer(_Context.GetDevice(), framebuffer, nullptr);

		for (auto imageView : _SwapChainImageViews)
			vkDestroyImageView(_Context.GetDevice(), imageView, nullptr);

		if (_SwapChain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(_Context.GetDevice(), _SwapChain, nullptr);
	}

	void VulkanSwapchain::Recreate()
	{
		int width = 0, height = 0;
		auto& window = Application::Get()->GetWindow();
		width = window.GetWidth();
		height = window.GetHeight();
		while (width == 0 || height == 0) {
			width = window.GetWidth();
			height = window.GetHeight();
			glfwWaitEvents();
		}

		_Context.WaitDeviceIdle();
		DestroySwapchain();
		CreateSwapChain();
		CreateDepthResources();
		CreateImageViews();
		CreateFramebuffers();
	}

	VkResult VulkanSwapchain::Submit(VkCommandBuffer* cmd, uint32_t* imageIndex)
	{
		VkSubmitInfo submitInfo{};
		VkSemaphore waitSemaphores[] = { _ImageAvailableSemaphores[_FrameIndex] };
		VkSemaphore signalSemaphores[] = { _RenderFinishedSemaphores[*imageIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT };
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmd;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(_Context.GetDevice(), 1, &_InFlightFences[_FrameIndex]);
		CHECKF(vkQueueSubmit(_Context.GetGraphicsQueue(), 1, &submitInfo, _InFlightFences[_FrameIndex]) != VK_SUCCESS, "Failed to swap buffers");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &_SwapChain;
		presentInfo.pImageIndices = imageIndex;

		VkResult result = vkQueuePresentKHR(_Context.GetPresentQueue(), &presentInfo);
		return result;
	}

	bool VulkanSwapchain::Swapbuffers(uint32_t* imageIndex)
	{
		vkWaitForFences(_Context.GetDevice(), 1, &_InFlightFences[_FrameIndex], VK_TRUE, UINT64_MAX);
		VkResult result = vkAcquireNextImageKHR(_Context.GetDevice(), _SwapChain, UINT64_MAX, _ImageAvailableSemaphores[_FrameIndex], VK_NULL_HANDLE, imageIndex);
		return (result != VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
	}
}