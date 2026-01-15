#include "hepch.h"
#include "VulkanPipeline.h"
#include "ShaderCompiler.h"
#include "Core/Application.h"
#include "Renderer/Model.h"

VulkanPipeline::VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath)
	: _VulkanContext(Application::Get()->GetVulkanContext())
{
	_VulkanShader = MEM::Ref<VulkanShader>::Create(vertexPath, fragmentPath);

	auto bindingDescription = Vertex::GetBindingDescription();
	auto attributeDescriptons = Vertex::GetAttributeDescriptions();
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptons.size());
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptons.data();
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = _VulkanShader->GetVertexModule();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = _VulkanShader->GetFragmentModule();
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = 2;
	createInfo.pStages = shaderStages;
	createInfo.pVertexInputState =   &vertexInputInfo;
	createInfo.pInputAssemblyState = &config.InputAssembyCreateInfo;
	createInfo.pViewportState =		 &config.ViewportStateCreateInfo;
	createInfo.pRasterizationState = &config.ResterizationStateCreateInfo;
	createInfo.pMultisampleState =   &config.MultisampleStateCreateInfo;
	createInfo.pDepthStencilState =  &config.DepthStencilCreateInfo;
	createInfo.pColorBlendState =	 &config.ColorBlendStateCreateInfo;
	createInfo.pDynamicState =		 &config.DynamicStateCreateInfo;
	createInfo.layout =				 config.PipelineLayout;
	createInfo.renderPass =			 config.RenderPass;
	createInfo.subpass = 0;
	CHECKF(vkCreateGraphicsPipelines(_VulkanContext.GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_VulkanPipeline) != VK_SUCCESS, "Failed to create pipeline");
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipeline(_VulkanContext.GetDevice(), _VulkanPipeline, nullptr);
}

void VulkanPipeline::Bind(VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _VulkanPipeline);
}