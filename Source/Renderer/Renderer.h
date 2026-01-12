#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanSwapchain.h"
#include "Vulkan/VulkanCommands.h"
#include "PipelineLibrary.h"
#include "Core/Memory.h"
#include <functional>
#include <array>

class Renderer
{
public:
	static void Init();
	static void Destroy();
	static void BeginFrame();
	static void EndFrame();

	static uint32_t GetFrameIndex();
	static MEM::Ref<PipelineLibrary>& GetPipelineLibrary();
private:
	//VkCommandBuffer BeginRecordCommandBuffer();
	//void EndRecordCommandBuffer();

	//MEM::Scope<VulkanDescriptorPool> _DescriptorPool;
	//std::array<MEM::Scope<VulkanDescriptorSetLayout>, 2> _DescriptorSetLayouts;

	//MEM::Scope<VulkanBuffer> _UniformBuffer;
	//VkDescriptorSet _TextureDescriptorSet;
	//VkDescriptorSet _UniformDescriptorSet; TODO - Move to scene renderer
};

