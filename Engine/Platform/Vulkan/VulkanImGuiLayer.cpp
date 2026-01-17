#include "hepch.h"
#include "VulkanImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "VulkanRenderAPI.h"

namespace House {
	VulkanImGuiLayer::VulkanImGuiLayer() : ImGuiLayer("VulkanImGuiLayer") { }
	VulkanImGuiLayer::~VulkanImGuiLayer()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplVulkan_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImGuiLayer::OnAttach()
	{
		_DescriptorPool = VulkanDescriptorPool::Builder()
			.SetMaxSets(1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();

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
		initInfo.DescriptorPool = _DescriptorPool->GetDescriptorPool();
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = false;
		initInfo.PipelineInfoMain = pipelineInfo;

		ImGui_ImplVulkan_Init(&initInfo);
		ImGui_ImplVulkan_CreateMainPipeline(&pipelineInfo);
	}

	void VulkanImGuiLayer::OnDetach()
	{
	}

	void VulkanImGuiLayer::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
	void VulkanImGuiLayer::End()
	{
		auto cmd = dynamic_cast<VulkanRenderAPI*>(Renderer::GetAPI())->GetCurrentCommandBuffer();
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
	}
}