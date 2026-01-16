#pragma once
#include "Utilities/Memory.h"
#include "RenderAPI.h"

namespace House {
	class RenerCommands
	{
	public:
		static void BeginSwapchainRenderPass() {
			_RenderAPI->BeginSwapchainRenderPass();
		}
		static void EndSwapchainRenderPass() {
			_RenderAPI->EndSwapchainRenderPass();
		}
		static void SubmitSwapchain() {
			_RenderAPI->SubmitSwapchain();
		}
		static void DrawVertex(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount) {
			_RenderAPI->DrawVertex(pipeline, vertexBuffer, vertexCount);
		}
		static void DrawIndexed(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount) {
			_RenderAPI->DrawIndexed(pipeline, vertexBuffer, indexBuffer, indicesCount);
		}
	private:
		static RenderAPI* _RenderAPI;
	};
}