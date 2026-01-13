#include "hepch.h"
#include "PipelineLibrary.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

MEM::Ref<VulkanPipeline>& PipelineLibrary::AddPipeline(const std::string& name, const std::string& vertexPath, const std::string& fragPath, VulkanPipelineConfig& pipelineConfig)
{
	_Pipelines[name] = MEM::MakeRef<VulkanPipeline>(pipelineConfig, vertexPath, fragPath);
	return _Pipelines[name];
}

void PipelineLibrary::RemovePipeline(const std::string& name)
{
	_Pipelines[name] = nullptr;
	_Pipelines.erase(name);
}

MEM::Ref<VulkanPipeline>& PipelineLibrary::GetPipeline(const std::string& name) { return _Pipelines[name]; }
