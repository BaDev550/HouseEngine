#pragma once
#include "Utilities/Memory.h"
#include <array>

#include "VulkanDescriptorManager.h"
#include "Renderer/RenderPass.h"

namespace House {
	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(MEM::Ref<Pipeline>& pipeline);
		virtual ~VulkanRenderPass();
		
		virtual void Begin() override;
		virtual void End() override;

		virtual void SetInput(std::string_view name, const MEM::Ref<Buffer>& buffer) override;
		virtual void SetInput(std::string_view name, const MEM::Ref<Texture2D>& texture, uint32_t index = 0) override;

		virtual MEM::Ref<Framebuffer> GetFramebuffer() const override { return _Pipeline->GetPipelineData().Framebuffer; }
		MEM::Ref<VulkanPipeline>& GetPipeline() { return _Pipeline; }
		DescriptorManager* GetDescriptorManager() { return _DescriptorManager; }
	private:
		void BeginCustomFramebufferPass(
			VkCommandBuffer cmd, 
			MEM::Ref<Framebuffer>& framebuffer, 
			std::vector<VkRenderingAttachmentInfo>& colorAttachments, 
			VkRenderingAttachmentInfo& depthAttachment,
			VkExtent2D& extent
		);
		void BeginDefaultSwapchainPass(
			VkCommandBuffer cmd, 
			std::vector<VkRenderingAttachmentInfo>& colorAttachments, 
			VkRenderingAttachmentInfo& depthAttachment,
			VkExtent2D& extent
		);
		void EndCustomFramebufferPass(VkCommandBuffer cmd, MEM::Ref<Framebuffer>& framebuffer);
		void EndDefaultSwapchainPass(VkCommandBuffer cmd);

		MEM::Ref<VulkanPipeline> _Pipeline;
		DescriptorManager* _DescriptorManager;
	};
}