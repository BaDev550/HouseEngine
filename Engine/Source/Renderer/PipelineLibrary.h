#pragma once
#include "Vulkan/VulkanPipeline.h"
#include <unordered_map>
#include "Utilities/Memory.h"

class PipelineLibrary : public MEM::RefCounted
{
public:
	MEM::Ref<VulkanPipeline>& AddPipeline(const std::string& name, const std::string& vertexPath, const std::string& fragPath, VulkanPipelineConfig& pipelineConfig);
	MEM::Ref<VulkanPipeline>& GetPipeline(const std::string& name);
	void RemovePipeline(const std::string& name);
private:
	std::unordered_map<std::string, MEM::Ref<VulkanPipeline>> _Pipelines;
};

