#pragma once
#include "VulkanContext.h"

class VulkanSwapchain
{
public:
	size_t GetImageCount() const { return _SwapChainImages.size(); }

	VulkanSwapchain(VulkanContext* context);
	~VulkanSwapchain();
	VulkanSwapchain(const VulkanSwapchain&) = delete;
	VulkanSwapchain& operator=(VulkanSwapchain&) = delete;
	VkResult Submit(VkCommandBuffer* cmd, uint32_t* imageIndex);
	VkResult AcquireNextImage(uint32_t* imageIndex);
	void Recreate();

	VkSwapchainKHR GetSwapChain() const { return _SwapChain; }
	VkRenderPass GetRenderPass() const { return _RenderPass; }
	VkFramebuffer GetSwapchainFramebuffer(int index) const { return _SwapChainFramebuffers[index]; }
	VkExtent2D GetSwapChainExtent() const { return _SwapChainExtent; }
private:
	void CreateSwapChain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateFramebuffers();
	void CreateSyncObjects();
	void DestroySwapchain();

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
private:
	uint32_t _FrameIndex = 0;

	VkRenderPass _RenderPass;
	VkSwapchainKHR _SwapChain;
	VkExtent2D _SwapChainExtent;
	VkFormat _SwapChainImageFormat;
	std::vector<VkFramebuffer> _SwapChainFramebuffers;
	std::vector<VkImage> _SwapChainImages;
	std::vector<VkImageView> _SwapChainImageViews;
	std::vector<VkSemaphore> _ImageAvailableSemaphores;
	std::vector<VkSemaphore> _RenderFinishedSemaphores;
	std::vector<VkFence> _InFlightFences;

	VulkanContext& _Context;
};

