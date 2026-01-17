#pragma once
#include "Renderer/Swapchain.h"
#include "VulkanContext.h"

namespace House {
	class VulkanSwapchain : public Swapchain
	{
	public:
		size_t GetImageCount() const { return _SwapChainImages.size(); }

		VulkanSwapchain(VulkanContext* context);
		~VulkanSwapchain();
		VulkanSwapchain(const VulkanSwapchain&) = delete;
		VulkanSwapchain& operator=(VulkanSwapchain&) = delete;
		VkResult Submit(VkCommandBuffer* cmd, uint32_t* imageIndex);
		virtual bool Swapbuffers(uint32_t* imageIndex = nullptr) override;
		void Recreate();

		VkSwapchainKHR GetSwapChain() const { return _SwapChain; }
		VkRenderPass GetRenderPass() const { return _RenderPass; }
		VkFramebuffer GetSwapchainFramebuffer(int index) const { return _SwapChainFramebuffers[index]; }
		VkImageView GetSwapchainImageView(int index) const { return _SwapChainImageViews[index]; }
		VkImageView GetDepthImageView() const { return _DepthImageView; }
		VkExtent2D GetSwapChainExtent() const { return _SwapChainExtent; }
		VkFormat GetSwapChainFormat() const { return _SwapChainImageFormat; }
		VkImage GetSwapchainImage(int index) { return _SwapChainImages[index]; }
		VkImage GetDepthImage() const { return _DepthImage; }
	private:
		void CreateSwapChain();
		void CreateDepthResources();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();
		void DestroySwapchain();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkFormat FindDepthFormat();
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

		VkImage _DepthImage;
		VkDeviceMemory _DepthImageMemory;
		VkImageView _DepthImageView;

		VulkanContext& _Context;
	};
}