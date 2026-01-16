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

namespace House {
	class Renderer
	{
	public:
		static void Init();
		static void Destroy();

		static void BeginFrame();
		static void EndFrame();

		static void DrawMesh(MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform);

		static uint32_t GetDrawCall();
		static MEM::Ref<PipelineLibrary>& GetPipelineLibrary();
		static MEM::Ref<DescriptorManager>& GetDescriptorManager();
		static MEM::Ref<VulkanTexture>& GetWhiteTexture();
		static MEM::Ref<VulkanDescriptorPool>& GetDescriptorPool();
	};
}