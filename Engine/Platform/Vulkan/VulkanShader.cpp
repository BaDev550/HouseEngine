#include "hepch.h"
#include "VulkanShader.h"

namespace House {
	VulkanShader::VulkanShader(const std::string& vertexPath, const std::string& fragmentPath)
		: _VulkanContext(Application::Get()->GetRenderContext<VulkanContext>())
	{
		std::vector<char> vertShaderCode = ShaderCompiler::CompileShaderFileToSpirv(vertexPath, _CompiledData);
		std::vector<char> fragShaderCode = ShaderCompiler::CompileShaderFileToSpirv(fragmentPath, _CompiledData);
		CreateShaderModule(vertShaderCode, &_VertexShaderModule);
		CreateShaderModule(fragShaderCode, &_FragmentShaderModule);

		for (auto const& [set, bindings] : _CompiledData.ReflectData)
		{
			LOG_RENDERER_INFO("Set {0} Layout Creation: Found {1} bindings", set, bindings.size());
			auto builder = VulkanDescriptorSetLayout::Builder();
			for (auto const& [binding, info] : bindings)
			{
				LOG_RENDERER_INFO("  - Binding {0}: {1} {2} {3}", binding, info.Name, DescriptorInfo::ShaderReflectionDataTypeToString(info.Type), info.Count);
				VkDescriptorType vkType = ShaderCompiler::ShaderReflectionTypeToVulkanType(info.Type);
				builder.AddBinding(binding, vkType, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, info.Count);
			}
			_DescriptorLayouts[set] = builder.Build();
		}
	}

	VulkanShader::~VulkanShader()
	{
		_DescriptorLayouts.clear();
		vkDestroyShaderModule(_VulkanContext.GetDevice(), _FragmentShaderModule, nullptr);
		vkDestroyShaderModule(_VulkanContext.GetDevice(), _VertexShaderModule, nullptr);
	}

	void VulkanShader::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
		CHECKF(vkCreateShaderModule(_VulkanContext.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS, "failed to create shader module!");
	}
}