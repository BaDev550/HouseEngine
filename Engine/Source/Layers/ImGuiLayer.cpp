#include "ImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Core/Application.h"
#include "Renderer/Renderer.h"

ImGuiLayer::ImGuiLayer()
{
	auto& descriptorPool = Renderer::GetDescriptorPool();
    auto& context = Application::Get()->GetVulkanContext();
    auto& window = Application::Get()->GetWindow();

    VkRenderPass renderPass = window.GetSwapchain().GetRenderPass();
	VkFormat swapchainImageFormat = window.GetSwapchain().GetSwapChainFormat();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.GetHandle(), true);

	ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
	pipelineInfo.RenderPass = renderPass;
	pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	pipelineInfo.PipelineRenderingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
	pipelineInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;
	
    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = context.GetInstance();
    initInfo.PhysicalDevice = context.GetPhysicalDevice();
    initInfo.Device = context.GetDevice();
    initInfo.Queue = context.GetGraphicsQueue();
    initInfo.DescriptorPool = descriptorPool->GetDescriptorPool();
    initInfo.MinImageCount = 3;
    initInfo.ImageCount = 3;
    initInfo.UseDynamicRendering = false;
	initInfo.PipelineInfoMain = pipelineInfo;

    ImGui_ImplVulkan_Init(&initInfo);
	ImGui_ImplVulkan_CreateMainPipeline(&pipelineInfo);
}

ImGuiLayer::~ImGuiLayer() {
	ImGui_ImplVulkan_Shutdown();
}

void ImGuiLayer::BeginFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::EndFrame(VkCommandBuffer cmd) {
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}