#include "VulkanPipeline.h"
#include "ShaderCompiler.h"
#include "Application.h"

VulkanPipeline::VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath)
	: _VulkanContext(*Application::Get()->GetVulkanContext())
{
	std::vector<char> vertShaderCode = pipeline::utils::CompileShaderFileToSpirv(vertexPath);
	std::vector<char> fragShaderCode = pipeline::utils::CompileShaderFileToSpirv(fragmentPath);
	CreateShaderModule(vertShaderCode, &_VertexShaderModule);
	CreateShaderModule(fragShaderCode, &_FragmentShaderModule);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = _VertexShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = _FragmentShaderModule;
	fragShaderStageInfo.pName = "main";
	
	auto setLayouts = Application::Get()->GetSetLayouts();

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &setLayouts;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	
	if (vkCreatePipelineLayout(_VulkanContext.GetDevice(), &pipelineLayoutInfo, nullptr, &_PipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout!");

	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptons = Vertex::GetAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptons.size());
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptons.data();
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderStages;
	createInfo.pVertexInputState =   &vertexInputInfo;
	createInfo.pInputAssemblyState = &config.InputAssembyCreateInfo;
	createInfo.pViewportState =		 &config.ViewportStateCreateInfo;
	createInfo.pRasterizationState = &config.ResterizationStateCreateInfo;
	createInfo.pMultisampleState =   &config.MultisampleStateCreateInfo;
	createInfo.pDepthStencilState =  nullptr;
	createInfo.pColorBlendState =	 &config.ColorBlendStateCreateInfo;
	createInfo.pDynamicState =		 &config.DynamicStateCreateInfo;
	createInfo.layout =				 _PipelineLayout;
	createInfo.renderPass =			 config.RenderPass;
	createInfo.subpass = 0;
	if (vkCreateGraphicsPipelines(_VulkanContext.GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_VulkanPipeline) != VK_SUCCESS)
		throw std::runtime_error("Failed to create pipeline");
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipelineLayout(_VulkanContext.GetDevice(), _PipelineLayout, nullptr);
	vkDestroyShaderModule(_VulkanContext.GetDevice(), _FragmentShaderModule, nullptr);
	vkDestroyShaderModule(_VulkanContext.GetDevice(), _VertexShaderModule, nullptr);
	vkDestroyPipeline(_VulkanContext.GetDevice(), _VulkanPipeline, nullptr);
}

void VulkanPipeline::Bind(VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _VulkanPipeline);
}

void VulkanPipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	if (vkCreateShaderModule(_VulkanContext.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		throw std::runtime_error("failed to create shader module!");
}
