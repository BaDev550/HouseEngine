#include "hepch.h"
#include "VulkanRenderAPI.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

namespace House {
	struct FrameContext {
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	} static s_Frames[MAX_FRAMES_IN_FLIGHT];
	static bool s_FrameStarted = false;

	struct DrawData {
		uint32_t DrawCall;
	} s_DrawData;

	void VulkanRenderAPI::InsertImageMemoryBarrier(VkCommandBuffer cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageAspectFlags aspectMask)
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

	VkCommandBuffer VulkanRenderAPI::GetCurrentCommandBuffer() {
		CHECKF(!s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
		return s_Frames[Renderer::GetFrameIndex()].CommandBuffer;
	}

	void VulkanRenderAPI::Init()
	{
		auto& context = Application::Get()->GetVulkanContext();
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			auto& frame = s_Frames[i];

			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = context.FindPhysicalDeviceQueueFamilies().graphicsFamily.value();
			CHECKF((vkCreateCommandPool(context.GetDevice(), &poolInfo, nullptr, &frame.CommandPool) != VK_SUCCESS), "Failed to create command pool");

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = frame.CommandPool;
			allocInfo.commandBufferCount = 1;
			CHECKF((vkAllocateCommandBuffers(context.GetDevice(), &allocInfo, &frame.CommandBuffer) != VK_SUCCESS), "Failed to create command buffer");
		}
	}

	void VulkanRenderAPI::Destroy()
	{
		const auto& device = Application::Get()->GetVulkanContext().GetDevice();
		for (auto& frame : s_Frames) { vkDestroyCommandPool(device, frame.CommandPool, nullptr); }
	}

	void VulkanRenderAPI::BeginFrame()
	{
		CHECKF(s_FrameStarted, "Cant call beginframe while still processing a frame");
		s_FrameStarted = true;
		auto& window = Application::Get()->GetWindow();
		window.SwapBuffers();
		auto& swapchain = window.GetSwapchain();
		auto& imageIndex = window.GetImageIndex();
		VkImage swapChainImage = swapchain.GetSwapchainImage(imageIndex);
		VkImage swapChainDepthImage = swapchain.GetDepthImage();
		VkCommandBuffer cmd = GetCurrentCommandBuffer();
		FrameContext& frame = s_Frames[Renderer::GetFrameIndex()];
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		CHECKF((vkResetCommandPool(Application::Get()->GetVulkanContext().GetDevice(), frame.CommandPool, 0) != VK_SUCCESS), "Failed to reset command pool");
		CHECKF((vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS), "Failed to begin recording to command buffer");
		InsertImageMemoryBarrier(cmd, swapChainImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT);
		InsertImageMemoryBarrier(cmd, swapChainDepthImage,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanRenderAPI::EndFrame()
	{
		CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");
		VkCommandBuffer cmd = GetCurrentCommandBuffer();
		auto& window = Application::Get()->GetWindow();
		auto& swapchain = window.GetSwapchain();
		auto& imageIndex = window.GetImageIndex();
		VkImage swapChainImage = swapchain.GetSwapchainImage(imageIndex);

		InsertImageMemoryBarrier(cmd, swapChainImage,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 0,
			VK_IMAGE_ASPECT_COLOR_BIT);

		CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
		VkResult result = swapchain.Submit(&cmd, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.HasResized()) {
			window.ResetResizeFlag();
			swapchain.Recreate();
		}

		s_DrawData.DrawCall = 0;
		s_FrameStarted = false;
	}
	 
	void VulkanRenderAPI::CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size)
	{
		auto& device = Application::Get()->GetVulkanContext();
		const auto& vulkanSrcBuffer = srcBuffer.As<VulkanBuffer>();
		const auto& vulkanDstBuffer = dstBuffer.As<VulkanBuffer>();
		device.CopyBuffer(vulkanSrcBuffer->GetBuffer(), vulkanDstBuffer->GetBuffer(), size);
	}

	void VulkanRenderAPI::DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform)
	{
		auto cmd = GetCurrentCommandBuffer();
		auto vulkanRenderPass = renderPass.As<VulkanRenderPass>();
		const auto& vulkanPipeline = vulkanRenderPass->GetPipeline();
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline());
		VkDescriptorSet globalSet = vulkanRenderPass->GetDescriptorManager()->GetDescriptorSet(Renderer::GetFrameIndex(), 0);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 0, 1, &globalSet, 0, nullptr);
		vkCmdPushConstants(cmd, vulkanPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transform);
		for (const auto& mesh : model->GetMeshes()) {
			auto& material = model->GetMaterialByID(mesh.GetMaterialID());
			material->Bind();

			const auto& vulkanVertexBuffer = mesh.GetVertexBuffer().As<VulkanBuffer>();
			const auto& vulkanIndexBuffer = mesh.GetIndexBuffer().As<VulkanBuffer>();
			VkBuffer buffers[] = { vulkanVertexBuffer->GetBuffer()};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
			vkCmdBindIndexBuffer(cmd, vulkanIndexBuffer->GetBuffer(), offsets[0], VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1, 0, 0, 0);
			s_DrawData.DrawCall++;
		}
	}

	uint32_t VulkanRenderAPI::GetDrawCall() { return s_DrawData.DrawCall; }
}