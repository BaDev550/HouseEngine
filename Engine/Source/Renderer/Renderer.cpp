#include "hepch.h"
#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

struct RenderData {
	MEM::Ref<DescriptorAllocator> GlobalDescriptorAllocator;
	MEM::Ref<VulkanDescriptorSetLayout> GlobalLayout;
	MEM::Ref<VulkanDescriptorSetLayout> MaterialLayout;
	VkPipelineLayout PipelineLayout;
	uint32_t DrawCall = 0;

	void Reset() {
		DrawCall = 0;
	}
} s_Data;

struct FrameContext {
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
} static s_Frames[MAX_FRAMES_IN_FLIGHT];
static uint32_t s_FrameIndex;
static bool s_FrameStarted = false;

struct RenderContext {
	MEM::Ref<VulkanTexture> WhiteTexture = nullptr;
	MEM::Ref<PipelineLibrary> PipelineLibrary = nullptr;
} static s_Context;

VkCommandBuffer Renderer::GetCurrentCommandBuffer() {
	CHECKF(!s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
	return s_Frames[s_FrameIndex].CommandBuffer;
}

uint32_t Renderer::GetDrawCall() { return s_Data.DrawCall; }
uint32_t Renderer::GetFrameIndex() { return s_FrameIndex; }
MEM::Ref<PipelineLibrary>& Renderer::GetPipelineLibrary() { return s_Context.PipelineLibrary; }
MEM::Ref<DescriptorAllocator>& Renderer::GetDescriptorAllocator() { return s_Data.GlobalDescriptorAllocator; }

VkDescriptorSet Renderer::AllocateMaterialSet()
{
	return s_Data.GlobalDescriptorAllocator->Allocate(s_Data.MaterialLayout);
}

VkPipelineLayout Renderer::GetPipelineLayout()
{
	return s_Data.PipelineLayout;
}

MEM::Ref<VulkanTexture>& Renderer::GetWhiteTexture()
{
	return s_Context.WhiteTexture;
}

MEM::Ref<VulkanDescriptorPool>& Renderer::GetDescriptorPool()
{
	return s_Data.GlobalDescriptorAllocator->GetPool();
}

MEM::Ref<VulkanDescriptorSetLayout>& Renderer::GetGlobalDescriptorLayout()
{
	return s_Data.GlobalLayout;
}

MEM::Ref<VulkanDescriptorSetLayout>& Renderer::GetMaterialDescriptorLayout()
{
	return s_Data.MaterialLayout;
}

void Renderer::Init() {
	auto& context = Application::Get()->GetVulkanContext();

	uint32_t whiteTextureData = 0xffffffff;
	s_Context.PipelineLibrary = MEM::Ref<PipelineLibrary>::Create();
	s_Context.WhiteTexture = MEM::Ref<VulkanTexture>::Create(&whiteTextureData, 1, 1);
	s_Data.GlobalDescriptorAllocator = MEM::Ref<DescriptorAllocator>::Create();

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		auto& frame = s_Frames[i];

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = context.FindPhysicalDeviceQueueFamilies().graphicsFamily.value();
		CHECKF((vkCreateCommandPool(context.GetDevice(), &poolInfo, nullptr, &frame.CommandPool) != VK_SUCCESS), "Failed to create command pool");

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = frame.CommandPool;
		allocInfo.commandBufferCount = 1;
		CHECKF((vkAllocateCommandBuffers(context.GetDevice(), &allocInfo, &frame.CommandBuffer) != VK_SUCCESS), "Failed to create command buffer");
	}

	s_Data.GlobalLayout = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
		.Build();

	s_Data.MaterialLayout = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.Build();

	std::vector<VkDescriptorSetLayout> layouts = { s_Data.GlobalLayout->GetDescriptorSetLayout(), s_Data.MaterialLayout->GetDescriptorSetLayout() };
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
	CHECKF(vkCreatePipelineLayout(context.GetDevice(), &pipelineLayoutInfo, nullptr, &s_Data.PipelineLayout) != VK_SUCCESS, "failed to create pipeline layout!");

	VulkanPipelineConfig defaultConfig;
	VulkanContext::DefaultPipelineConfigInfo(defaultConfig);
	defaultConfig.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();
	defaultConfig.PipelineLayout = s_Data.PipelineLayout;

	Renderer::GetPipelineLibrary()->AddPipeline("MainPipeline", "Shaders/base.vert", "Shaders/base.frag", defaultConfig);
}

void Renderer::Destroy()
{
	auto device = Application::Get()->GetVulkanContext().GetDevice();

	s_Context.PipelineLibrary = nullptr;
	s_Context.WhiteTexture = nullptr;
	s_Data.GlobalLayout = nullptr;
	s_Data.MaterialLayout = nullptr;
	s_Data.GlobalDescriptorAllocator = nullptr;

	for (auto& frame : s_Frames) {
		vkDestroyCommandPool(device, frame.CommandPool, nullptr);
	}

	if (s_Data.PipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, s_Data.PipelineLayout, nullptr);
		s_Data.PipelineLayout = VK_NULL_HANDLE;
	}
}

VkCommandBuffer Renderer::BeginFrame() {
	CHECKF(s_FrameStarted, "Cant call beginframe while still processing a frame");
	Application::Get()->GetWindow().SwapBuffers();
	s_FrameStarted = true;
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	FrameContext& frame = s_Frames[s_FrameIndex];
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	CHECKF((vkResetCommandPool(Application::Get()->GetVulkanContext().GetDevice(), frame.CommandPool, 0) != VK_SUCCESS), "Failed to reset command pool");
	CHECKF((vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS), "Failed to begin recording to command buffer");
	return cmd;
}

void Renderer::EndFrame() {
	CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
	VulkanCommands::SubmitSwapchain(cmd);
	s_FrameStarted = false;
	s_FrameIndex = (s_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	s_Data.Reset();
}

void Renderer::RenderMesh(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform)
{
	pipeline->Bind(cmd);
	vkCmdPushConstants(cmd, s_Data.PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transform);
	for (const auto& mesh : model->GetMeshes()) {
		s_Data.DrawCall++;
		model->GetMaterialByID(mesh.GetMaterialID())->Bind(cmd, s_Data.PipelineLayout);
		VulkanCommands::DrawIndexed(cmd, pipeline, mesh.GetVertexBuffer(), mesh.GetIndexBuffer(), mesh.GetIndexCount());
	}
}
