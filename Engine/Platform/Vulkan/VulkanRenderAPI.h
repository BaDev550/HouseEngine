#pragma once
#include "Renderer/RenderAPI.h"

namespace House {
	class VulkanRenderAPI : public RenderAPI {
	public:
		virtual void Init() override;
		virtual void Destroy() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void DrawMesh(MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform) override;
		virtual void DrawVertex(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount) override;
		virtual void DrawIndexed(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount) override;
		virtual uint32_t GetDrawCall() override;
	private:
		VkCommandBuffer GetCurrentCommandBuffer();
	};
}