#include "hepch.h"
#include "VulkanPipeline.h"
#include "ShaderCompiler.h"
#include "Core/Application.h"
#include "Renderer/Model.h"

VulkanPipeline::VulkanPipeline(VulkanPipelineConfig& config, const std::string& vertexPath, const std::string& fragmentPath)
	: _VulkanContext(Application::Get()->GetVulkanContext())
{
	_Shader = MEM::Ref<VulkanShader>::Create(vertexPath, fragmentPath);

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
	vertShaderStageInfo.module = _Shader->GetVertexModule();
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = _Shader->GetFragmentModule();
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	std::vector<VkDescriptorSetLayout> layouts;
	const auto& layoutMap = _Shader->GetDesciptorLayouts();
	for (auto const& [set, layout] : layoutMap) { layouts.push_back(layout->GetDescriptorSetLayout()); }

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	VkPushConstantRange push_constant;
	push_constant.offset = 0;
	push_constant.size = sizeof(glm::mat4);
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &push_constant;
	CHECKF(vkCreatePipelineLayout(_VulkanContext.GetDevice(), &pipelineLayoutInfo, nullptr, &_PipelineLayout) != VK_SUCCESS, "failed to create pipeline layout!");

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
	createInfo.layout =				 _PipelineLayout;
	createInfo.renderPass =			 config.RenderPass;
	createInfo.subpass = 0;
	CHECKF(vkCreateGraphicsPipelines(_VulkanContext.GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_Pipeline) != VK_SUCCESS, "Failed to create pipeline");
}

VulkanPipeline::~VulkanPipeline()
{
	vkDestroyPipelineLayout(_VulkanContext.GetDevice(), _PipelineLayout, nullptr);
	vkDestroyPipeline(_VulkanContext.GetDevice(), _Pipeline, nullptr);
}

void VulkanPipeline::Bind(VkCommandBuffer cmd)
{
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline);
}