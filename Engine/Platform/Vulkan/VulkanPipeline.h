#pragma once
#include "VulkanContext.h"

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath);
	~VulkanPipeline();
	VulkanPipeline(const VulkanPipeline&) = delete;
	VulkanPipeline& operator=(const VulkanPipeline&) = delete;
	
	void Bind(VkCommandBuffer cmd);
private:
	void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	VkPipeline _VulkanPipeline;
	VkShaderModule _VertexShaderModule;
	VkShaderModule _FragmentShaderModule;
	
	VulkanContext& _VulkanContext;
};

/*
	TODO List: make a diffirent shader class apart from pipeline,
		Make a shader reflector with spriv
		Implement everythink to renderpass
*/

