#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanSwapchain.h"
#include "PipelineLibrary.h"
#include "Core/Memory.h"
#include <functional>
#include <array>

struct FrameContext {
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
};

class Renderer
{
public:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(Renderer&) = delete;
	
	std::vector<VkDescriptorSetLayout> GetSetLayouts() {
		std::vector<VkDescriptorSetLayout> result(_DescriptorSetLayouts.size());
		for (size_t i = 0; i < _DescriptorSetLayouts.size(); i++)
			result[i] = _DescriptorSetLayouts[i]->GetDescriptorSetLayout();
		return result;
	}

	void Submit(std::function<void(VkCommandBuffer& commandBuffer)> fn);
	VkExtent2D GetSwapChainExtent() const { return _Swapchain->GetSwapChainExtent(); }
	VkRenderPass GetSwapChainRenderPass() const { return _Swapchain->GetRenderPass(); }
	uint32_t GetImageIndex() const { return _ImageIndex; }
	uint32_t GetFrameIndex() const { return _FrameIndex; }
private:
	VkCommandBuffer BeginRecordCommandBuffer();
	void EndRecordCommandBuffer();
	void BeginSwapchainRenderPass(VkCommandBuffer cmd);
	void EndSwapchainRenderPass(VkCommandBuffer cmd);

	void CreateFrameContext();
	void DestroyFrameContext();
	VkCommandBuffer GetCurrentCommandBuffer();

	MEM::Ref<VulkanSwapchain> _Swapchain;
	std::array<FrameContext, MAX_FRAMES_IN_FLIGHT> _Frames;
	
	bool _FrameStarted = false;
	uint32_t _FrameIndex = 0;
	uint32_t _ImageIndex = 0;

	VulkanContext& _Context;
	Window& _Window;

	MEM::Scope<VulkanDescriptorPool> _DescriptorPool;
	std::array<MEM::Scope<VulkanDescriptorSetLayout>, 2> _DescriptorSetLayouts;

	MEM::Scope<VulkanBuffer> _UniformBuffer;
	VkDescriptorSet _TextureDescriptorSet;
	VkDescriptorSet _UniformDescriptorSet;
};

