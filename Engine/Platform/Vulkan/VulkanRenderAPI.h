#pragma once
#include "Renderer/RenderAPI.h"

namespace House {
	class VulkanRenderAPI : public RenderAPI {
	public:
		virtual void Init() override;
		virtual void Destroy() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size) override;
		virtual void DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform) override;
		virtual uint32_t GetDrawCall() override;
	public:
		VkCommandBuffer GetCurrentCommandBuffer();
	};
}