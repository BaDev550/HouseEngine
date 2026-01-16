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

		virtual void SetInput(std::string_view name, MEM::Ref<Buffer>& buffer) override;
		virtual void SetInput(std::string_view name, MEM::Ref<Texture2D>& texture) override;

		MEM::Ref<VulkanPipeline>& GetPipeline() { return _Pipeline; }
		DescriptorManager* GetDescriptorManager() { return _DescriptorManager; }
	private:
		MEM::Ref<VulkanPipeline> _Pipeline;
		DescriptorManager* _DescriptorManager;
	};
}