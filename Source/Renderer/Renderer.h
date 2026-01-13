#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanSwapchain.h"
#include "Vulkan/VulkanCommands.h"
#include "DescriptorAllocator.h"
#include "PipelineLibrary.h"
#include "Core/Memory.h"
#include <functional>
#include <array>

class Renderer
{
public:
	static void Init();
	static void Destroy();

	static VkCommandBuffer BeginFrame();
	static void EndFrame();

	static void RenderMesh(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, const glm::mat4& transform);

	static uint32_t GetDrawCall();
	static uint32_t GetFrameIndex();
	static MEM::Ref<PipelineLibrary>& GetPipelineLibrary();
	static MEM::Ref<DescriptorAllocator>& GetDescriptorAllocator();
public:
	static VkCommandBuffer GetCurrentCommandBuffer();
	static VkDescriptorSet AllocateMaterialSet();
	static VkPipelineLayout GetPipelineLayout();
	static MEM::Ref<VulkanTexture>& GetWhiteTexture();
	static MEM::Ref<VulkanDescriptorPool>& GetDescriptorPool();
	static MEM::Ref<VulkanDescriptorSetLayout>& GetMaterialDescriptorLayout();
	static MEM::Ref<VulkanDescriptorSetLayout>& GetGlobalDescriptorLayout();
};

