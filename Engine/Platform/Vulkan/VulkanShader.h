#pragma once
#include "VulkanContext.h"
#include "VulkanDescriptor.h"
#include "ShaderCompiler.h"
#include "Renderer/Shader.h"
#include <iostream>
#include <map>

namespace House {
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertexPath, const std::string& fragmentPath);
		~VulkanShader();
		VulkanShader(const VulkanShader&) = delete;
		VulkanShader& operator=(const VulkanShader&) = delete;

		VkShaderModule GetFragmentModule() const { return _FragmentShaderModule; }
		VkShaderModule GetVertexModule() const { return _VertexShaderModule; }
		MEM::Ref<VulkanDescriptorSetLayout>& GetDescriptorLayout(uint32_t set) { return _DescriptorLayouts[set]; }

		const CompiledShaderInfo& GetCompiledShaderData() const { return _CompiledData; }
		const std::unordered_map<std::string, ShaderBuffer>& GetShaderBuffers() const { return _CompiledData.Buffers; }
		const std::map<uint32_t, MEM::Ref<VulkanDescriptorSetLayout>>& GetDesciptorLayouts() const { return _DescriptorLayouts; }
		virtual std::map<uint32_t, std::map<uint32_t, DescriptorInfo>>& GetReflectData() override { return _CompiledData.ReflectData; }
	private:
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
		VkShaderModule _VertexShaderModule;
		VkShaderModule _FragmentShaderModule;
		CompiledShaderInfo _CompiledData;
		std::map<uint32_t, MEM::Ref<VulkanDescriptorSetLayout>> _DescriptorLayouts;

		VulkanContext& _VulkanContext;
	};
}