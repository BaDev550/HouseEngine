#pragma once
#include "VulkanContext.h"
#include "VulkanShader.h"
#include "Utilities/Memory.h"

class VulkanPipeline : public MEM::RefCounted
{
public:
	VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath);
	~VulkanPipeline();
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;
	
	MEM::Ref<VulkanShader>& GetShader() { return _VulkanShader; }

	void Bind(VkCommandBuffer cmd);
private:
	VkPipeline _VulkanPipeline;
	MEM::Ref<VulkanShader> _VulkanShader;
	
	VulkanContext& _VulkanContext;
};

