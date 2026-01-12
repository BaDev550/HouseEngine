#include "hepch.h"
#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

Renderer::Renderer()
	: _Context(Application::Get()->GetVulkanContext())
{
	CreateFrameContext();

	_DescriptorPool = VulkanDescriptorPool::Builder()
		.SetMaxSets(1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.Build();
	_DescriptorSetLayouts[0] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.Build();
	_DescriptorSetLayouts[1] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.Build();

	VulkanPipelineConfig config;
	VulkanContext::DefaultPipelineConfigInfo(config);
	config.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();
	PipelineLibrary::AddPipeline("MainPipeline", "Shaders/base.vert", "Shaders/base.frag", config);

	//VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	//_UniformBuffer = MEM::MakeScope<VulkanBuffer>(bufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//_UniformBuffer->Map();

	//VkDescriptorBufferInfo camInfo = _UniformBuffer->DescriptorInfo();
	//VkDescriptorImageInfo imageInfo = _Texture->GetImageDescriptorInfo();
	//VulkanDescriptorWriter(*_DescriptorSetLayouts[0], *_DescriptorPool)
	//	.WriteBuffer(0, &camInfo)
	//	.Build(_UniformDescriptorSet);
	//VulkanDescriptorWriter(*_DescriptorSetLayouts[1], *_DescriptorPool)
	//	.WriteImage(0, &imageInfo)
	//	.Build(_TextureDescriptorSet);
}

Renderer::~Renderer()
{
	DestroyFrameContext();
}

void Renderer::Submit(std::function<void(VkCommandBuffer& commandBuffer)> fn)
{
	VkCommandBuffer buffer = BeginRecordCommandBuffer();
	VulkanCommands::BeginSwapchainRenderPass(buffer);
	fn(buffer);
	VulkanCommands::EndSwapchainRenderPass(buffer);
	EndRecordCommandBuffer();
}

VkCommandBuffer Renderer::GetCurrentCommandBuffer() { 
	assert(_FrameStarted && "Cannot get a command buffer while frame is not in progress");
	return _Frames[_FrameIndex].CommandBuffer; 
}

VkCommandBuffer Renderer::BeginRecordCommandBuffer()
{
	assert(!_FrameStarted && "Cant call beginframe while still processing a frame");
	Application::Get()->GetWindow().SwapBuffers();

	_FrameStarted = true;

	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	FrameContext& frame = _Frames[_FrameIndex];
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	CHECKF((vkResetCommandPool(_Context.GetDevice(), frame.CommandPool, 0) != VK_SUCCESS), "Failed to reset command pool");
	CHECKF((vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS), "Failed to begin recording to command buffer");
	return cmd;
}

void Renderer::EndRecordCommandBuffer()
{
	assert(_FrameStarted && "Cant submit a not recorded frame");
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	CHECKF((vkEndCommandBuffer(cmd) != VK_SUCCESS), "Failed to record commad buffer");
	VulkanCommands::SubmitSwapchain(cmd);
	_FrameStarted = false;
	_FrameIndex = (_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::CreateFrameContext()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		auto& frame = _Frames[i];

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _Context.FindPhysicalDeviceQueueFamilies().graphicsFamily.value();
		CHECKF((vkCreateCommandPool(_Context.GetDevice(), &poolInfo, nullptr, &frame.CommandPool) != VK_SUCCESS), "Failed to create command pool");

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = frame.CommandPool;
		allocInfo.commandBufferCount = 1;
		CHECKF((vkAllocateCommandBuffers(_Context.GetDevice(), &allocInfo, &frame.CommandBuffer) != VK_SUCCESS), "Failed to create command buffer");
	}
}

void Renderer::DestroyFrameContext()
{
	for (auto& frame : _Frames) {
		vkFreeCommandBuffers(_Context.GetDevice(), frame.CommandPool, 1, &frame.CommandBuffer);
		vkDestroyCommandPool(_Context.GetDevice(), frame.CommandPool, nullptr);
	}
}
