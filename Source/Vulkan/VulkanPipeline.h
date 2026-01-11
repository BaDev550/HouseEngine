#pragma once
#include "VulkanContext.h"

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath);
	~VulkanPipeline();
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;
	VkPipelineLayout GetPipelineLayout() const { return _PipelineLayout; }

	void Bind(VkCommandBuffer cmd);
private:
	void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	VkPipeline _VulkanPipeline;
	VkPipelineLayout _PipelineLayout;
	VkShaderModule _VertexShaderModule;
	VkShaderModule _FragmentShaderModule;

	VulkanContext& _VulkanContext;
};

