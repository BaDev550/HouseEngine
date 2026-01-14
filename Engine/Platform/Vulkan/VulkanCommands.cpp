#include "hepch.h"
#include "VulkanCommands.h"
#include "Core/Application.h"

void VulkanCommands::BeginSwapchainRenderPass(VkCommandBuffer cmd)
{
	auto& swapchain = Application::Get()->GetWindow().GetSwapchain();
	auto& imageIndex = Application::Get()->GetWindow().GetImageIndex();
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = swapchain.GetRenderPass();
	renderPassInfo.framebuffer = swapchain.GetSwapchainFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = swapchain.GetSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0] = { 0.1f, 0.1f, 0.1f, 1.0f };
	clearValues[1] = { 1.0f, 0.0f };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain.GetSwapChainExtent().width);
	viewport.height = static_cast<float>(swapchain.GetSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmd, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain.GetSwapChainExtent();
	vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void VulkanCommands::EndSwapchainRenderPass(VkCommandBuffer cmd)
{
	vkCmdEndRenderPass(cmd);
}

void VulkanCommands::SubmitSwapchain(VkCommandBuffer cmd)
{
	auto& window = Application::Get()->GetWindow();
	auto& swapchain = window.GetSwapchain();
	auto& imageIndex = window.GetImageIndex();
	VkResult result = swapchain.Submit(&cmd, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.HasResized()) {
		window.ResetResizeFlag();
		swapchain.Recreate();
	}
}

void VulkanCommands::DrawVertex(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount)
{
	pipeline->Bind(cmd);
	VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
	vkCmdDraw(cmd, vertexCount, 1, 0, 0);
}

void VulkanCommands::DrawIndexed(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount)
{
	pipeline->Bind(cmd);
	VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(cmd, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmd, indicesCount, 1, 0, 0, 0);
}
