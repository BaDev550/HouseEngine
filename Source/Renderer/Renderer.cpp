#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

Renderer::Renderer()
	: _Context(*Application::Get()->GetVulkanContext()), _Window(*Application::Get()->GetWindow())
{
	_Swapchain = std::make_shared<VulkanSwapchain>();
	CreateFrameContext();
}

Renderer::~Renderer()
{
	_Swapchain = nullptr;
	DestroyFrameContext();
}

void Renderer::Submit(std::function<void(VkCommandBuffer& commandBuffer)> fn)
{
	VkCommandBuffer buffer = BeginRecordCommandBuffer();
	BeginSwapchainRenderPass(buffer);
	fn(buffer);
	EndSwapchainRenderPass(buffer);
	EndRecordCommandBuffer();
}

VkCommandBuffer Renderer::GetCurrentCommandBuffer() { 
	assert(_FrameStarted && "Cannot get a command buffer while frame is not in progress");
	return _Frames[_FrameIndex].CommandBuffer; 
}

VkCommandBuffer Renderer::BeginRecordCommandBuffer()
{
	assert(!_FrameStarted && "Cant call beginframe while still processing a frame");
	VkResult result = _Swapchain->AcquireNextImage(&_ImageIndex);
	if (result != VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("Failed to acquire next image");

	_FrameStarted = true;

	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	FrameContext& frame = _Frames[_FrameIndex];
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkResetCommandPool(_Context.GetDevice(), frame.CommandPool, 0) != VK_SUCCESS) throw std::runtime_error("Failed to reset command pool");
	if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) throw std::runtime_error("Failed to begin recording to command buffer");
	return cmd;
}

void Renderer::EndRecordCommandBuffer()
{
	assert(_FrameStarted && "Cant submit a not recorded frame");
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
		throw std::runtime_error("Failed to record commad buffer");
	VkResult result = _Swapchain->Submit(&cmd, &_ImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _Window.HasResized()) {
		_Window.ResetResizeFlag();
		_Swapchain->Recreate();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	_FrameStarted = false;
	_FrameIndex = (_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::BeginSwapchainRenderPass(VkCommandBuffer cmd)
{
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = _Swapchain->GetRenderPass();
	renderPassInfo.framebuffer = _Swapchain->GetSwapchainFramebuffer(_ImageIndex);
	renderPassInfo.renderArea.offset = { 0,0 };
	renderPassInfo.renderArea.extent = _Swapchain->GetSwapChainExtent();

	VkClearValue clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(_Swapchain->GetSwapChainExtent().width);
	viewport.height = static_cast<float>(_Swapchain->GetSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmd, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = _Swapchain->GetSwapChainExtent();
	vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void Renderer::EndSwapchainRenderPass(VkCommandBuffer cmd)
{
	vkCmdEndRenderPass(cmd);
}

void Renderer::CreateFrameContext()
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		auto& frame = _Frames[i];

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = _Context.FindPhysicalDeviceQueueFamilies().graphicsFamily.value();
		if (vkCreateCommandPool(_Context.GetDevice(), &poolInfo, nullptr, &frame.CommandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool");

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = frame.CommandPool;
		allocInfo.commandBufferCount = 1;
		if (vkAllocateCommandBuffers(_Context.GetDevice(), &allocInfo, &frame.CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command buffer");
	}
}

void Renderer::DestroyFrameContext()
{
	for (auto& frame : _Frames) {
		vkFreeCommandBuffers(_Context.GetDevice(), frame.CommandPool, 1, &frame.CommandBuffer);
		vkDestroyCommandPool(_Context.GetDevice(), frame.CommandPool, nullptr);
	}
}
