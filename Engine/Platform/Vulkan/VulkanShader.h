#pragma once
#include "VulkanContext.h"
#include "ShaderCompiler.h"
#include <iostream>
#include <map>

class VulkanShader
{
public:
	VulkanShader(const std::string& vertexPath, const std::string& fragmentPath);
	~VulkanShader();
	VulkanShader(const VulkanShader&) = delete;
	VulkanShader& operator=(const VulkanShader&) = delete;

	VkShaderModule GetFragmentModule() { return _FragmentShaderModule; }
	VkShaderModule GetVertexModule() { return _VertexShaderModule; }
	VkDescriptorSetLayout GetDescriptorLayout(uint32_t set) { return _DescriptorLayouts[set]; }
	const std::map<uint32_t, VkDescriptorSetLayout>& GetDesciptorLayouts() const { return _DescriptorLayouts; }
	const std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& GetReflectData() const { return _ReflectData; }
private:
	void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	VkShaderModule _VertexShaderModule;
	VkShaderModule _FragmentShaderModule;
	std::map<uint32_t, std::map<uint32_t, DescriptorInfo>> _ReflectData;
	std::map<uint32_t, VkDescriptorSetLayout> _DescriptorLayouts;

	VulkanContext& _VulkanContext;
};

