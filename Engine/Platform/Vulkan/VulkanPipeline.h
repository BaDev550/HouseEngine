#pragma once
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "Renderer/Pipeline.h"

namespace House {
	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineData& data);
		~VulkanPipeline();
		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;

		VkPipeline GetVulkanPipeline() const { return _Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return _PipelineLayout; }
		virtual PipelineData& GetPipelineData() override { return _Data; }
		virtual const PipelineData& GetPipelineData() const override { return _Data; }

		virtual void Invalidate() override;
		virtual MEM::Ref<Shader> GetShader() const override { return _Data.Shader; }
	private:
		VkPipeline _Pipeline;
		VkPipelineLayout _PipelineLayout;

		PipelineData _Data;
		VulkanContext& _Context;
	};
}