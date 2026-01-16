#pragma once

namespace House {
	enum class GrapichsAPI {
		Vulkan,
		OpenGL
	};

	class RenderAPI {
	public:
		virtual void BeginSwapchainRenderPass() = 0;
		virtual void EndSwapchainRenderPass() = 0;
		virtual void SubmitSwapchain() = 0;
		virtual void DrawVertex(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount) = 0;
		virtual void DrawIndexed(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount) = 0;
	};
}