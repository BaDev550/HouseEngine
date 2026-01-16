#pragma once
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanSwapchain.h"
#include "Vulkan/VulkanCommands.h"
#include "Vulkan/VulkanDescriptorManager.h"
#include "PipelineLibrary.h"
#include "Utilities/Memory.h"
#include <functional>
#include <array>

class Renderer
{
public:
	static void Init();
	static void Destroy();

	static VkCommandBuffer BeginFrame();
	static void EndFrame();

	template<typename FuncT>
	static void Submit(FuncT&& func) {}

	static void RenderMesh(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform);

	static uint32_t GetDrawCall();
	static uint32_t GetFrameIndex();
	static MEM::Ref<PipelineLibrary>& GetPipelineLibrary();
	static MEM::Ref<DescriptorManager>& GetDescriptorManager();
public:
	static VkCommandBuffer GetCurrentCommandBuffer();
	static MEM::Ref<VulkanTexture>& GetWhiteTexture();
	static MEM::Ref<VulkanDescriptorPool>& GetDescriptorPool();
};

