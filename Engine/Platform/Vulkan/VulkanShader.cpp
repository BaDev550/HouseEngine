#include "hepch.h"
#include "VulkanShader.h"

VulkanShader::VulkanShader(const std::string& vertexPath, const std::string& fragmentPath)
	: _VulkanContext(Application::Get()->GetVulkanContext())
{
	std::vector<char> vertShaderCode = ShaderCompiler::CompileShaderFileToSpirv(vertexPath, _ReflectData);
	std::vector<char> fragShaderCode = ShaderCompiler::CompileShaderFileToSpirv(fragmentPath, _ReflectData);
	CreateShaderModule(vertShaderCode, &_VertexShaderModule);
	CreateShaderModule(fragShaderCode, &_FragmentShaderModule);
}

VulkanShader::~VulkanShader()
{
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
