#include "hepch.h"
#include "VulkanRenderAPI.h"

namespace House {
	struct FrameContext {
		VkCommandPool CommandPool;
		VkCommandBuffer CommandBuffer;
	} static s_Frames[MAX_FRAMES_IN_FLIGHT];
	static uint32_t s_FrameIndex;
	static bool s_FrameStarted = false;

	struct DrawData {
		uint32_t DrawCall;
	} s_DrawData;

	VkCommandBuffer VulkanRenderAPI::GetCurrentCommandBuffer() {
		CHECKF(!s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
		return s_Frames[s_FrameIndex].CommandBuffer;
	}

	void VulkanRenderAPI::Init()
	{
		auto& context = Application::Get()->GetVulkanContext();
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
		const auto& device = Application::Get()->GetVulkanContext().GetDevice();
		for (auto& frame : s_Frames) { vkDestroyCommandPool(device, frame.CommandPool, nullptr); }
	}

	void VulkanRenderAPI::BeginFrame()
	{
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
	}

	void VulkanRenderAPI::EndFrame()
	{
		CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");
		VkCommandBuffer cmd = GetCurrentCommandBuffer();
		CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
		VulkanCommands::SubmitSwapchain(cmd);
		s_DrawData.DrawCall = 0;
		s_FrameStarted = false;
		s_FrameIndex = (s_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderAPI::DrawMesh(MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform)
	{
		auto cmd = GetCurrentCommandBuffer();
		pipeline->Bind(cmd);
		//VkDescriptorSet globalSet = s_Data.GlobalDescriptorManager->GetDescriptorSet(s_FrameIndex, 0);
		//vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &globalSet, 0, nullptr);
		vkCmdPushConstants(cmd, pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transform);
		for (const auto& mesh : model->GetMeshes()) {
			s_DrawData.DrawCall++;
			model->GetMaterialByID(mesh.GetMaterialID())->Bind(cmd);
			VulkanCommands::DrawIndexed(cmd, pipeline, mesh.GetVertexBuffer(), mesh.GetIndexBuffer(), mesh.GetIndexCount());
		}
	}

	void VulkanRenderAPI::DrawVertex(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount)
	{

	}

	void VulkanRenderAPI::DrawIndexed(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount)
	{

	}

	uint32_t VulkanRenderAPI::GetDrawCall() { return s_DrawData.DrawCall; }
}