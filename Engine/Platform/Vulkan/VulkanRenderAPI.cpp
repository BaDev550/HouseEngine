#include "hepch.h"
#include "VulkanRenderAPI.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"

namespace House {
	struct FrameContext {
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	} static s_Frames[MAX_FRAMES_IN_FLIGHT];
	static bool s_FrameStarted = false;
	static RenderStats s_RenderStats;

	static std::chrono::system_clock::time_point s_FrameStartTime;

	bool VulkanRenderAPI::FrameStarted() const
	{
		return s_FrameStarted;
	}

	VkCommandBuffer VulkanRenderAPI::GetCurrentCommandBuffer() {
		CHECKF(!s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
		return s_Frames[Renderer::GetFrameIndex()].CommandBuffer;
	}

	void VulkanRenderAPI::Init()
	{
		auto& context = Application::Get()->GetRenderContext<VulkanContext>();
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
	}

	void VulkanRenderAPI::Destroy()
	{
		const auto& device = Application::Get()->GetRenderContext<VulkanContext>().GetDevice();
		for (auto& frame : s_Frames) { vkDestroyCommandPool(device, frame.CommandPool, nullptr); }
	}

	void VulkanRenderAPI::BeginFrame()
	{
		CHECKF(s_FrameStarted, "Cant call beginframe while still processing a frame");
		s_FrameStartTime = std::chrono::system_clock::now();
		s_FrameStarted = true;
		auto& window = Application::Get()->GetWindow();
		window.SwapBuffers();
		auto& swapchain = *dynamic_cast<VulkanSwapchain*>(&window.GetSwapchain());
		auto& imageIndex = window.GetImageIndex();
		auto frameIndex = Application::Get()->GetFrameIndex();
		VkImage swapChainImage = swapchain.GetSwapchainImage(imageIndex);
		VkImage swapChainDepthImage = swapchain.GetDepthImage();
		VkCommandBuffer cmd = GetCurrentCommandBuffer();
		FrameContext& frame = s_Frames[Renderer::GetFrameIndex()];
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		CHECKF((vkResetCommandPool(Application::Get()->GetRenderContext<VulkanContext>().GetDevice(), frame.CommandPool, 0) != VK_SUCCESS), "Failed to reset command pool");
		CHECKF((vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS), "Failed to begin recording to command buffer");
	}

	bool VulkanRenderAPI::EndFrame()
	{
		CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");

		VkCommandBuffer cmd = GetCurrentCommandBuffer();
		auto& window = Application::Get()->GetWindow();
		auto& swapchain = *dynamic_cast<VulkanSwapchain*>(&window.GetSwapchain());
		auto& imageIndex = window.GetImageIndex();
		VkImage swapChainImage = swapchain.GetSwapchainImage(imageIndex);

		CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
		VkResult result = swapchain.Submit(&cmd, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.HasResized()) {
			window.ResetResizeFlag();
			swapchain.Recreate();
		}
		auto end = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - s_FrameStartTime);

		s_RenderStats.FrameTime = elapsed.count() / 1000.0f;
		s_RenderStats.DrawCall = 0;
		s_RenderStats.TriangleCount = 0;
		s_FrameStarted = false;
		return true;
	}
	 
	void VulkanRenderAPI::CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size)
	{
		auto& device = Application::Get()->GetRenderContext<VulkanContext>();
		const auto& vulkanSrcBuffer = srcBuffer.As<VulkanBuffer>();
		const auto& vulkanDstBuffer = dstBuffer.As<VulkanBuffer>();
		device.CopyBuffer(vulkanSrcBuffer->GetBuffer(), vulkanDstBuffer->GetBuffer(), size);
	}

	void VulkanRenderAPI::DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform)
	{
		auto cmd = GetCurrentCommandBuffer();

		MeshPushConstants pc_data;
		auto vulkanRenderPass = renderPass.As<VulkanRenderPass>();
		const auto& vulkanPipeline = vulkanRenderPass->GetPipeline();
		pc_data.Transform = transform;
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline());
		vkCmdPushConstants(cmd, vulkanPipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MeshPushConstants), &pc_data);

		for (const auto& mesh : model->GetMeshes()) {
			model->GetMaterialByID(mesh.GetMaterialID())->Bind();

			const auto& vulkanVertexBuffer = mesh.GetVertexBuffer().As<VulkanBuffer>();
			const auto& vulkanIndexBuffer = mesh.GetIndexBuffer().As<VulkanBuffer>();
			VkBuffer buffers[] = { vulkanVertexBuffer->GetBuffer()};
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);
			vkCmdBindIndexBuffer(cmd, vulkanIndexBuffer->GetBuffer(), offsets[0], VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmd, mesh.GetIndexCount(), 1, 0, 0, 0);

			s_RenderStats.DrawCall++;
			s_RenderStats.TriangleCount += mesh.GetIndexCount() / 3;
		}
	}

	void VulkanRenderAPI::DrawFullscreenQuad(MEM::Ref<RenderPass>& renderPass)
	{
		auto cmd = GetCurrentCommandBuffer();
		auto vulkanRenderPass = renderPass.As<VulkanRenderPass>();
		const auto& vulkanPipeline = vulkanRenderPass->GetPipeline();

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetVulkanPipeline());

		VkDescriptorSet globalSet = vulkanRenderPass->GetDescriptorManager()->GetDescriptorSet(Renderer::GetFrameIndex(), 0);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->GetPipelineLayout(), 0, 1, &globalSet, 0, nullptr);
		vkCmdDraw(cmd, 3, 1, 0, 0);
		s_RenderStats.DrawCall++;
	}

	void VulkanRenderAPI::ResetRenderState()
	{
		Application::Get()->GetWindow().GetRenderContext().WaitDeviceIdle();

		if (s_FrameStarted) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0;
			beginInfo.pInheritanceInfo = nullptr;

			vkResetCommandPool(Application::Get()->GetRenderContext<VulkanContext>().GetDevice(), s_Frames[Renderer::GetFrameIndex()].CommandPool, 0);
			CHECKF((vkBeginCommandBuffer(s_Frames[Renderer::GetFrameIndex()].CommandBuffer, &beginInfo) != VK_SUCCESS), "Failed to begin recording to command buffer");
		}
	}

	RenderStats VulkanRenderAPI::GetRenderStats() { return s_RenderStats; }
}