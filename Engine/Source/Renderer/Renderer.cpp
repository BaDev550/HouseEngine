#include "hepch.h"
#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

struct RenderData {
	MEM::Ref<VulkanTexture> WhiteTexture = nullptr;
	MEM::Ref<PipelineLibrary> PipelineLibrary = nullptr;
	MEM::Ref<DescriptorManager> GlobalDescriptorManager;

	uint32_t DrawCall = 0;

	void Reset() { DrawCall = 0; }
} s_Data;

struct FrameContext {
	VkCommandPool CommandPool;
	VkCommandBuffer CommandBuffer;
} static s_Frames[MAX_FRAMES_IN_FLIGHT];
static uint32_t s_FrameIndex;
static bool s_FrameStarted = false;

VkCommandBuffer Renderer::GetCurrentCommandBuffer() {
	CHECKF(!s_FrameStarted, "Cannot get a command buffer while frame is not in progress");
	return s_Frames[s_FrameIndex].CommandBuffer;
}

uint32_t Renderer::GetDrawCall() { return s_Data.DrawCall; }
uint32_t Renderer::GetFrameIndex() { return s_FrameIndex; }
MEM::Ref<PipelineLibrary>& Renderer::GetPipelineLibrary() { return s_Data.PipelineLibrary; }
MEM::Ref<DescriptorManager>& Renderer::GetDescriptorManager() { return s_Data.GlobalDescriptorManager; }

MEM::Ref<VulkanTexture>& Renderer::GetWhiteTexture()
{
	return s_Data.WhiteTexture;
}

MEM::Ref<VulkanDescriptorPool>& Renderer::GetDescriptorPool()
{
	return s_Data.GlobalDescriptorManager->GetPool();
}

void Renderer::Init() {
	auto& context = Application::Get()->GetVulkanContext();

	uint32_t whiteTextureData = 0xffffffff;
	s_Data.PipelineLibrary = MEM::Ref<PipelineLibrary>::Create();
	s_Data.WhiteTexture = MEM::Ref<VulkanTexture>::Create(&whiteTextureData, 1, 1);

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

	VulkanPipelineConfig defaultConfig;
	VulkanContext::DefaultPipelineConfigInfo(defaultConfig);
	defaultConfig.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();

	Renderer::GetPipelineLibrary()->AddPipeline("MainPipeline", "Shaders/base.vert", "Shaders/base.frag", defaultConfig);

	DescriptorManagerSpecification specs{};
	specs.Pipeline = Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline");
	s_Data.GlobalDescriptorManager = MEM::Ref<DescriptorManager>::Create(specs);
}

void Renderer::Destroy()
{
	auto device = Application::Get()->GetVulkanContext().GetDevice();

	s_Data.GlobalDescriptorManager = nullptr;
	s_Data.PipelineLibrary = nullptr;
	s_Data.WhiteTexture = nullptr;

	for (auto& frame : s_Frames) { vkDestroyCommandPool(device, frame.CommandPool, nullptr); }
}

VkCommandBuffer Renderer::BeginFrame() {
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
	return cmd;
}

void Renderer::EndFrame() {
	CHECKF(!s_FrameStarted, "Cant submit a not recorded frame");
	VkCommandBuffer cmd = GetCurrentCommandBuffer();
	CHECKF(vkEndCommandBuffer(cmd) != VK_SUCCESS, "Failed to record commad buffer");
	VulkanCommands::SubmitSwapchain(cmd);
	s_FrameStarted = false;
	s_FrameIndex = (s_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	s_Data.Reset();
}

void Renderer::RenderMesh(VkCommandBuffer cmd, MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform)
{
	pipeline->Bind(cmd);
	VkDescriptorSet globalSet = s_Data.GlobalDescriptorManager->GetDescriptorSet(s_FrameIndex, 0);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipelineLayout(), 0, 1, &globalSet, 0, nullptr);
	vkCmdPushConstants(cmd, pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transform);
	for (const auto& mesh : model->GetMeshes()) {
		s_Data.DrawCall++;
		model->GetMaterialByID(mesh.GetMaterialID())->Bind(cmd);
		VulkanCommands::DrawIndexed(cmd, pipeline, mesh.GetVertexBuffer(), mesh.GetIndexBuffer(), mesh.GetIndexCount());
	}
}
