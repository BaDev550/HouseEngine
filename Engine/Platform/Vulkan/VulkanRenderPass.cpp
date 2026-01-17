#include "hepch.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderAPI.h"
#include "VulkanBuffer.h"

namespace House {
	namespace Utils {
		bool HasStencilComponent(VkFormat Format)
		{
			return ((Format == VK_FORMAT_D32_SFLOAT_S8_UINT) ||
				(Format == VK_FORMAT_D24_UNORM_S8_UINT));
		}

		// Copied from the "3D Graphics Rendering Cookbook"
		void ImageMemBarrier(VkCommandBuffer CmdBuf, VkImage Image, VkFormat Format,
			VkImageLayout OldLayout, VkImageLayout NewLayout, int LayerCount)
		{
			VkImageMemoryBarrier barrier = {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				.pNext = NULL,
				.srcAccessMask = 0,
				.dstAccessMask = 0,
				.oldLayout = OldLayout,
				.newLayout = NewLayout,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = Image,
				.subresourceRange = VkImageSubresourceRange {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = (uint32_t)LayerCount
				}
			};

			VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_NONE;
			VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_NONE;

			if (NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
				(Format == VK_FORMAT_D16_UNORM) ||
				(Format == VK_FORMAT_X8_D24_UNORM_PACK32) ||
				(Format == VK_FORMAT_D32_SFLOAT) ||
				(Format == VK_FORMAT_S8_UINT) ||
				(Format == VK_FORMAT_D16_UNORM_S8_UINT) ||
				(Format == VK_FORMAT_D24_UNORM_S8_UINT))
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (HasStencilComponent(Format)) {
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}
			else {
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_GENERAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}

			if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
				NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			} /* Convert back from read-only to updateable */
			else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			} /* Convert from updateable texture to shader read-only */
			else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
				NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			} /* Convert depth texture from undefined state to depth-stencil buffer */
			else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			} /* Wait for render pass to complete */
			else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = 0; // VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = 0;
				/*
						sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				///		destinationStage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
						destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				*/
				sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			} /* Convert back from read-only to color attachment */
			else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			} /* Convert from updateable texture to shader read-only */
			else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			} /* Convert back from read-only to depth attachment */
			else if (OldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				destinationStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			} /* Convert from updateable depth texture to shader read-only */
			else if (OldLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
				barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

				sourceStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
				barrier.srcAccessMask = 0;
				barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

				sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			}
			else if (OldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
				barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				barrier.dstAccessMask = 0;

				sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			}
			else {
				printf("Unknown barrier case\n");
				exit(1);
			}

			vkCmdPipelineBarrier(CmdBuf, sourceStage, destinationStage,
				0, 0, NULL, 0, NULL, 1, &barrier);
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
		PipelineData pipelineData = _Pipeline->GetPipelineData();

		VkExtent2D extent{};
		std::vector<VkRenderingAttachmentInfo> colorAttachments{};
		VkRenderingAttachmentInfo depthAttachment{};

		if (pipelineData.Framebuffer) { BeginCustomFramebufferPass(cmd, pipelineData.Framebuffer, colorAttachments, depthAttachment, extent); }
		else { BeginDefaultSwapchainPass(cmd, colorAttachments, depthAttachment, extent); }

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { {0, 0}, extent };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
		renderingInfo.pColorAttachments = colorAttachments.data();
		renderingInfo.pDepthAttachment = &depthAttachment;

		vkCmdBeginRendering(cmd, &renderingInfo);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline->GetVulkanPipeline());

		VkViewport viewport{ 0, 0, (float)extent.width, (float)extent.height };
		viewport.minDepth = 0;
		viewport.maxDepth = 1;
		vkCmdSetViewport(cmd, 0, 1, &viewport);
		VkRect2D scissor{ {0, 0}, extent };
		vkCmdSetScissor(cmd, 0, 1, &scissor);

		_DescriptorManager->UpdateSets(Renderer::GetFrameIndex());
	}

	void VulkanRenderPass::End()
	{
		auto cmd = dynamic_cast<VulkanRenderAPI*>(Renderer::GetAPI())->GetCurrentCommandBuffer();
		vkCmdEndRendering(cmd);

		PipelineData pipelineData = _Pipeline->GetPipelineData();
		if (pipelineData.Framebuffer) { EndCustomFramebufferPass(cmd, pipelineData.Framebuffer); }
		else { EndDefaultSwapchainPass(cmd); }
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

	void VulkanRenderPass::BeginCustomFramebufferPass(
		VkCommandBuffer cmd, 
		MEM::Ref<Framebuffer>& framebuffer, 
		std::vector<VkRenderingAttachmentInfo>& colorAttachments, 
		VkRenderingAttachmentInfo& depthAttachment, 
		VkExtent2D& extent
	)
	{
		const FramebufferSpecification fbSpecs = framebuffer->GetSpecification();

		const float DepthClearColor = fbSpecs.DepthClearValue;
		const glm::vec4 clearColor = fbSpecs.ClearColor;
		const MEM::Ref<VulkanTexture>& vulkanAttachmentDepth = framebuffer->GetDepthTextureAttachment().As<VulkanTexture>();
		extent = { framebuffer->GetWidth(), framebuffer->GetHeight() };
		uint32_t attachmentCount = framebuffer->GetAttachmentCount();

		framebuffer->Bind();
		colorAttachments.resize(attachmentCount);
		for (int i = 0; i < colorAttachments.size(); i++) {
			const MEM::Ref<VulkanTexture>& vulkanAttachmentTexture = framebuffer->GetAttachmentTexture(i).As<VulkanTexture>();
			VkImage fbImage = vulkanAttachmentTexture->GetImage();
			VkImage fbDepthImage = vulkanAttachmentDepth->GetImage();
			VkFormat fbformat = vulkanAttachmentTexture->GetFormat();

			colorAttachments[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			colorAttachments[i].imageView = vulkanAttachmentTexture->GetImageView();
			colorAttachments[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachments[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachments[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachments[i].clearValue = { {{clearColor.x, clearColor.y, clearColor.z, clearColor.a}} };
			Utils::ImageMemBarrier(cmd, fbImage, fbformat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
		}

		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView = vulkanAttachmentDepth->GetImageView();
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.clearValue = { DepthClearColor, 0 };
	}

	void VulkanRenderPass::BeginDefaultSwapchainPass(VkCommandBuffer cmd, std::vector<VkRenderingAttachmentInfo>& colorAttachments, VkRenderingAttachmentInfo& depthAttachment, VkExtent2D& extent)
	{
		auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
		VkImage swapChainImage = swapchain.GetSwapchainImage(Application::Get()->GetWindow().GetImageIndex());
		VkImage swapChainDepthImage = swapchain.GetDepthImage();
		VkFormat swapchainFormat = swapchain.GetSwapChainFormat();
		extent = swapchain.GetSwapChainExtent();

		colorAttachments.resize(1);
		colorAttachments[0].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachments[0].imageView = swapchain.GetSwapchainImageView(Application::Get()->GetWindow().GetImageIndex());
		colorAttachments[0].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachments[0].clearValue = { {{0.1f, 0.1f, 0.1f, 1.0f}} };

		depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		depthAttachment.imageView = swapchain.GetDepthImageView();
		depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.clearValue = { 1.0f, 0 };

		Utils::ImageMemBarrier(cmd, swapChainImage, swapchainFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
	}

	void VulkanRenderPass::EndCustomFramebufferPass(VkCommandBuffer cmd, MEM::Ref<Framebuffer>& framebuffer)
	{
		uint32_t attachmentCount = framebuffer->GetAttachmentCount();
		const FramebufferSpecification fbSpecs = framebuffer->GetSpecification();
		for (int i = 0; i < attachmentCount; i++) {
			const MEM::Ref<VulkanTexture>& vulkanAttachmentTexture = framebuffer->GetAttachmentTexture(i).As<VulkanTexture>();
			VkImage fbImage = vulkanAttachmentTexture->GetImage();
			VkFormat fbformat = vulkanAttachmentTexture->GetFormat();
			Utils::ImageMemBarrier(cmd, fbImage, fbformat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
		}
		framebuffer->Unbind();
	}

	void VulkanRenderPass::EndDefaultSwapchainPass(VkCommandBuffer cmd)
	{
		auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
		VkImage swapChainImage = swapchain.GetSwapchainImage(Application::Get()->GetWindow().GetImageIndex());
		VkFormat swapChainFormat = swapchain.GetSwapChainFormat();
		Utils::ImageMemBarrier(cmd, swapChainImage, swapChainFormat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);
	}
}