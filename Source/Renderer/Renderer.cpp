#include "hepch.h"
#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>


struct FrameContext {
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
} static s_Frames[MAX_FRAMES_IN_FLIGHT];
static uint32_t s_FrameIndex;
static bool s_FrameStarted = false;

struct RenderContext {
	MEM::Ref<PipelineLibrary> PipelineLibrary = nullptr;
	MEM::Scope<VulkanDescriptorPool> DescriptorPool = nullptr;
	std::vector<MEM::Scope<VulkanDescriptorSetLayout>> DescriptorSetLayouts;
} static s_Context;

VkCommandBuffer GetCurrentCommandBuffer() {
	CHECKF(s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
	return s_Frames[s_FrameIndex].CommandBuffer;
}

uint32_t Renderer::GetFrameIndex() { return s_FrameIndex; }
MEM::Ref<PipelineLibrary>& Renderer::GetPipelineLibrary() { return s_Context.PipelineLibrary; }
MEM::Scope<VulkanDescriptorPool>& Renderer::GetDescriptorPool() { return s_Context.DescriptorPool; }
MEM::Scope<VulkanDescriptorSetLayout>& Renderer::GetVulkanDescriptorSetLayout(int index) { return s_Context.DescriptorSetLayouts[index]; }

std::vector<VkDescriptorSetLayout>& Renderer::GetDescriptorSetLayouts() 
{ 
	std::vector<VkDescriptorSetLayout> result(s_Context.DescriptorSetLayouts.size());
	for (size_t i = 0; i < s_Context.DescriptorSetLayouts.size(); i++)
		result[i] = s_Context.DescriptorSetLayouts[i]->GetDescriptorSetLayout();
	return result;
}

void Renderer::Init() {
	auto& context = Application::Get()->GetVulkanContext();

	VulkanPipelineConfig defaultConfig;
	VulkanContext::DefaultPipelineConfigInfo(defaultConfig);
	defaultConfig.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();
	s_Context.PipelineLibrary = MEM::MakeRef<PipelineLibrary>();

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

	s_Context.DescriptorPool = VulkanDescriptorPool::Builder()
		.SetMaxSets(1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.Build();
	s_Context.DescriptorSetLayouts.resize(2);
	s_Context.DescriptorSetLayouts[0] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.Build();
	s_Context.DescriptorSetLayouts[1] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.Build();

	Renderer::GetPipelineLibrary()->AddPipeline("MainPipeline", "Shaders/base.vert", "Shaders/base.frag", defaultConfig);
}

void Renderer::Destroy()
{
	for (auto& frame : s_Frames) {
		vkFreeCommandBuffers(Application::Get()->GetVulkanContext().GetDevice(), frame.CommandPool, 1, &frame.CommandBuffer);
		vkDestroyCommandPool(Application::Get()->GetVulkanContext().GetDevice(), frame.CommandPool, nullptr);
	}
}

void Renderer::BeginFrame() {
	CHECKF(!s_FrameStarted, "Cant call beginframe while still processing a frame");
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
}

void Renderer::EndFrame() {
	CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
	VulkanCommands::SubmitSwapchain(cmd);
	s_FrameStarted = false;
	s_FrameIndex = (s_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

	//CreateFrameContext();

	//_DescriptorPool = VulkanDescriptorPool::Builder()
	//	.SetMaxSets(1000)
	//	.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
	//	.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
	//	.Build();
	//_DescriptorSetLayouts[0] = VulkanDescriptorSetLayout::Builder()
	//	.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
	//	.Build();
	//_DescriptorSetLayouts[1] = VulkanDescriptorSetLayout::Builder()
	//	.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
	//	.Build();

	//VulkanPipelineConfig config;
	//VulkanContext::DefaultPipelineConfigInfo(config);
	//config.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();
	//PipelineLibrary::AddPipeline("MainPipeline", "Shaders/base.vert", "Shaders/base.frag", config);

	////VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	////_UniformBuffer = MEM::MakeScope<VulkanBuffer>(bufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	////_UniformBuffer->Map();

	////VkDescriptorBufferInfo camInfo = _UniformBuffer->DescriptorInfo();
	////VkDescriptorImageInfo imageInfo = _Texture->GetImageDescriptorInfo();
	////VulkanDescriptorWriter(*_DescriptorSetLayouts[0], *_DescriptorPool)
	////	.WriteBuffer(0, &camInfo)
	////	.Build(_UniformDescriptorSet);
	////VulkanDescriptorWriter(*_DescriptorSetLayouts[1], *_DescriptorPool)
	////	.WriteImage(0, &imageInfo)
	////	.Build(_TextureDescriptorSet); TODO MOVE TO SCENE RENDERER
