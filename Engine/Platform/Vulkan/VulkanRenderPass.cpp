#include "hepch.h"
#include "VulkanRenderPass.h"
#include "VulkanRenderAPI.h"
#include "VulkanBuffer.h"

namespace House {
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
		vkCmdEndRendering(cmd);
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