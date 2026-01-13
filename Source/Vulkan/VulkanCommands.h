#pragma once
#include "VulkanContext.h"
#include "VulkanPipeline.h"
#include "Renderer/Model.h"

class VulkanCommands
{
public:
	static void BeginSwapchainRenderPass(VkCommandBuffer cmd);
	static void EndSwapchainRenderPass(VkCommandBuffer cmd);
	static void SubmitSwapchain(VkCommandBuffer cmd);
	static void DrawVertex(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount);
	static void DrawIndexed(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount);
};