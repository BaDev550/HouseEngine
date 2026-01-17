#include "hepch.h"
#include "VulkanPipeline.h"
#include "ShaderCompiler.h"
#include "Core/Application.h"
#include "Renderer/Model.h"

namespace House {
	VulkanPipeline::VulkanPipeline(const PipelineData& data)
		: _Context(Application::Get()->GetVulkanContext()), _Data(data)
	{
		Invalidate();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		vkDestroyPipelineLayout(_Context.GetDevice(), _PipelineLayout, nullptr);
		vkDestroyPipeline(_Context.GetDevice(), _Pipeline, nullptr);
	}

	void VulkanPipeline::Invalidate()
	{
		VulkanPipelineConfig config{};
		VulkanContext::DefaultPipelineConfigInfo(config);
		const auto& vulkanShader = _Data.Shader.As<VulkanShader>();
		const auto& compiledData = vulkanShader->GetCompiledShaderData();
		
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(compiledData.ArrtibDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexAttributeDescriptions = compiledData.ArrtibDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = &compiledData.BindingDescription;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vulkanShader->GetVertexModule();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = vulkanShader->GetFragmentModule();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		std::vector<VkDescriptorSetLayout> layouts;
		const auto& layoutMap = vulkanShader->GetDesciptorLayouts();
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
		CHECKF(vkCreatePipelineLayout(_Context.GetDevice(), &pipelineLayoutInfo, nullptr, &_PipelineLayout) != VK_SUCCESS, "failed to create pipeline layout!");

		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
		VkFormat colorFormat = Application::Get()->GetWindow().GetSwapchain().GetSwapChainFormat();
		pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfo.colorAttachmentCount = 1;
		pipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;
		pipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pNext = &pipelineRenderingCreateInfo;
		createInfo.stageCount = 2;
		createInfo.pStages = shaderStages;
		createInfo.pVertexInputState = &vertexInputInfo;
		createInfo.pInputAssemblyState = &config.InputAssembyCreateInfo;
		createInfo.pViewportState = &config.ViewportStateCreateInfo;
		createInfo.pRasterizationState = &config.ResterizationStateCreateInfo;
		createInfo.pMultisampleState = &config.MultisampleStateCreateInfo;
		createInfo.pDepthStencilState = &config.DepthStencilCreateInfo;
		createInfo.pColorBlendState = &config.ColorBlendStateCreateInfo;
		createInfo.pDynamicState = &config.DynamicStateCreateInfo;
		createInfo.layout = _PipelineLayout;

		createInfo.renderPass = VK_NULL_HANDLE;
		createInfo.subpass = 0;
		CHECKF(vkCreateGraphicsPipelines(_Context.GetDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &_Pipeline) != VK_SUCCESS, "Failed to create pipeline");
	}

#if 0
	void VulkanPipeline::Bind(VkCommandBuffer cmd)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _Pipeline);
	}
#endif
}