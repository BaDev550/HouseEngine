#include "hepch.h"
#include "VulkanImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Core/Application.h"
#include "Renderer/Renderer.h"
#include "VulkanRenderAPI.h"
#include "VulkanSwapchain.h"

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

		auto& context = Application::Get()->GetRenderContext<VulkanContext>();
		auto& window = Application::Get()->GetWindow();

		VkFormat swapchainImageFormat = dynamic_cast<VulkanSwapchain*>(&window.GetSwapchain())->GetSwapChainFormat();

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForVulkan(window.GetHandle(), true);

		ImGui_ImplVulkan_PipelineInfo pipelineInfo{};
		pipelineInfo.RenderPass = VK_NULL_HANDLE;
		pipelineInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineInfo.PipelineRenderingCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		pipelineInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		pipelineInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;
		pipelineInfo.PipelineRenderingCreateInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = context.GetInstance();
		initInfo.PhysicalDevice = context.GetPhysicalDevice();
		initInfo.Device = context.GetDevice();
		initInfo.Queue = context.GetGraphicsQueue();
		initInfo.DescriptorPool = _DescriptorPool->GetDescriptorPool();
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = true;
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

		auto& swapchain = *dynamic_cast<VulkanSwapchain*>(&Application::Get()->GetWindow().GetSwapchain());
		VkExtent2D extent = swapchain.GetSwapChainExtent();

		VkRenderingAttachmentInfo colorAttachment{};
		colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		colorAttachment.imageView = swapchain.GetSwapchainImageView(Application::Get()->GetWindow().GetImageIndex());
		colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		VkRenderingInfo renderingInfo{};
		renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		renderingInfo.renderArea = { {0, 0}, extent };
		renderingInfo.layerCount = 1;
		renderingInfo.colorAttachmentCount = 1;
		renderingInfo.pColorAttachments = &colorAttachment;

		vkCmdBeginRendering(cmd, &renderingInfo);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
		vkCmdEndRendering(cmd);
	}
}