#pragma once
#include "Vulkan/VulkanPipeline.h"
#include <unordered_map>
#include "Core/Memory.h"

class PipelineLibrary
{
public:
	static MEM::Ref<VulkanPipeline>& AddPipeline(const std::string& name, const std::string& vertexPath, const std::string& fragPath, VulkanPipelineConfig& pipelineConfig);
	static VulkanPipeline* GetPipeline(const std::string& name);
	static void RemovePipeline(const std::string& name);
private:
	static std::unordered_map<std::string, MEM::Ref<VulkanPipeline>> _Pipelines;
};

