#include "hepch.h"
#include "VulkanCommands.h"
#include "Core/Application.h"

/*
		_UniformBufferObject.Model = glm::rotate(glm::mat4(1.0f), _DeltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_UniformBufferObject.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		_UniformBufferObject.Proj = glm::perspective(glm::radians(45.0f), (float)_Renderer->GetSwapChainExtent().width / (float)_Renderer->GetSwapChainExtent().height, 0.01f, 10.0f);
		_UniformBuffer->WriteToBuffer(&_UniformBufferObject);
	UBO Setting ref
*/

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

	VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
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
	CHECKF((result != VK_SUCCESS), "failed to acquire swap chain image!");
}

void VulkanCommands::DrawVertex(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<VulkanBuffer> vertexBuffer, uint32_t vertexCount)
{
	pipeline->Bind(cmd);

	VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
	vkCmdBindVertexBuffers(cmd, 0, 1, buffers, 0);
	vkCmdDraw(cmd, vertexCount, 1, 0, 0);
}

void VulkanCommands::DrawIndexed(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<VulkanBuffer> indexBuffer, uint32_t indicesCount)
{
	pipeline->Bind(cmd);

	vkCmdBindIndexBuffer(cmd, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmd, indicesCount, 1, 0, 0, 0);
}

void VulkanCommands::DrawModelWithMaterial(VkCommandBuffer cmd, MEM::Ref<Model>& model)
{
	for (const auto& mesh : model->GetMeshes()) {
		// TODO - bind descriptor sets and send to shader
		/*
			_Pipeline->Bind(cmd);

			VkBuffer vertexBuffers[] = { _VertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_Pipeline->GetPipelineLayout(),
				0, 1,
				&_UniformDescriptorSet,
				0, nullptr);
			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_Pipeline->GetPipelineLayout(),
				1, 1,
				&_TextureDescriptorSet,
				0, nullptr);
			vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(cmd, _IndexBuffer->GetBuffer(), offsets[0], VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		REF
		*/
	}
}
