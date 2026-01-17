#include "hepch.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderAPI.h"
#include "VulkanBuffer.h"

namespace House {
	namespace Utils {
		void ImageMemoryBarrier(
			VkCommandBuffer cmd, 
			VkImage image, 
			VkImageLayout oldLayout, 
			VkImageLayout newLayout, 
			VkPipelineStageFlags srcStageMask, 
			VkPipelineStageFlags dstStageMask, 
			VkAccessFlags srcAccessMask, 
			VkAccessFlags dstAccessMask, 
			VkImageAspectFlags aspectMask)
		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = aspectMask;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = srcAccessMask;
			barrier.dstAccessMask = dstAccessMask;
			vkCmdPipelineBarrier(
				cmd,
				srcStageMask,
				dstStageMask,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
		}
	}
	VulkanRenderPass::VulkanRenderPass(MEM::Ref<Pipeline>& pipeline)
		: _Pipeline(pipeline)
	{
		DescriptorManagerSpecification specs{};
		specs.Pipeline = _Pipeline;
		_DescriptorManager = new DescriptorManager(specs);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		delete _DescriptorManager;
		_DescriptorManager = nullptr;
	}

	void VulkanRenderPass::Begin()
	{
		auto cmd = dynamic_cast<VulkanRenderAPI*>(Renderer::GetAPI())->GetCurrentCommandBuffer();
		auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
		VkImage swapChainImage = swapchain.GetSwapchainImage(Application::Get()->GetWindow().GetImageIndex());
		VkImage swapChainDepthImage = swapchain.GetDepthImage();
		VkExtent2D extent = swapchain.GetSwapChainExtent();
		VkRenderingAttachmentInfo colorAttachments{};
		colorAttachments.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachments.imageView = swapchain.GetSwapchainImageView(Application::Get()->GetWindow().GetImageIndex());
		colorAttachments.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments.clearValue = { {{0.1f, 0.1f, 0.1f, 1.0f}} };

		VkRenderingAttachmentInfo depthAttachment{};
		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView = swapchain.GetDepthImageView();
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.clearValue = { 1.0f, 0 };

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { {0, 0}, extent };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachments;
		renderingInfo.pDepthAttachment = &depthAttachment;

		Utils::ImageMemoryBarrier(cmd, swapChainImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		Utils::ImageMemoryBarrier(cmd, swapChainDepthImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);

		vkCmdBeginRendering(cmd, &renderingInfo);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline->GetVulkanPipeline());

		VkViewport viewport{ 0, 0, (float)extent.width, (float)extent.height };
		vkCmdSetViewport(cmd, 0, 1, &viewport);
		VkRect2D scissor{ {0, 0}, extent };
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		_DescriptorManager->UpdateSets(Renderer::GetFrameIndex());
	}

	void VulkanRenderPass::End()
	{
		auto cmd = dynamic_cast<VulkanRenderAPI*>(Renderer::GetAPI())->GetCurrentCommandBuffer();
		auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
		VkImage swapChainImage = swapchain.GetSwapchainImage(Application::Get()->GetWindow().GetImageIndex());
		vkCmdEndRendering(cmd);

		Utils::ImageMemoryBarrier(cmd, swapChainImage,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
			VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanRenderPass::SetInput(std::string_view name, MEM::Ref<Buffer>& buffer)
	{
		auto vulkanBuffer = buffer.As<VulkanBuffer>();
		_DescriptorManager->WriteInput(name, vulkanBuffer);
	}
	void VulkanRenderPass::SetInput(std::string_view name, MEM::Ref<Texture2D>& texture)
	{
		auto vulkanTexture = texture.As<VulkanTexture>();
		_DescriptorManager->WriteInput(name, vulkanTexture);
	}
}