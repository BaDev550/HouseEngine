#pragma once
#include "Renderer/RenderAPI.h"
#include "VulkanDescriptor.h"

namespace House {
	class VulkanRenderAPI : public RenderAPI {
	public:
		virtual void Init() override;
		virtual void Destroy() override;

		virtual void BeginFrame() override;
		virtual bool EndFrame() override;

		virtual void CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size) override;
		virtual void DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform) override;
		virtual void DrawIndexed(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& vertexBuffer, MEM::Ref<Buffer>& indexBuffer, uint32_t count) override;
		virtual void DrawVertex(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& buffer, uint32_t count) override;
		virtual void DrawFullscreenQuad(MEM::Ref<RenderPass>& renderPass) override;
		virtual void ResetRenderState() override;
		virtual RenderStats GetRenderStats() override;
		virtual bool FrameStarted() const override;
	public:
		VkCommandBuffer GetCurrentCommandBuffer();
	};
}