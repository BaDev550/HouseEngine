#include "hepch.h"
#include "Renderer.h"
#include "RenderAPI.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

#include "Vulkan/VulkanRenderAPI.h"

namespace House {
	struct RenderData {
		MEM::Ref<VulkanTexture> WhiteTexture = nullptr;
		MEM::Ref<PipelineLibrary> PipelineLibrary = nullptr;
		MEM::Ref<DescriptorManager> GlobalDescriptorManager;
	} s_Data;
	static RenderAPI* s_RenderAPI = nullptr;

	RenderAPI* CreateRenderAPI() {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return new VulkanRenderAPI();
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}

	uint32_t Renderer::GetDrawCall() { return s_RenderAPI->GetDrawCall(); }
	MEM::Ref<PipelineLibrary>& Renderer::GetPipelineLibrary() { return s_Data.PipelineLibrary; }
	MEM::Ref<DescriptorManager>& Renderer::GetDescriptorManager() { return s_Data.GlobalDescriptorManager; }
	MEM::Ref<VulkanTexture>& Renderer::GetWhiteTexture() { return s_Data.WhiteTexture; }
	MEM::Ref<VulkanDescriptorPool>& Renderer::GetDescriptorPool() { return s_Data.GlobalDescriptorManager->GetPool(); }

	void Renderer::Init() {

		s_RenderAPI = CreateRenderAPI();
		s_RenderAPI->Init();

		uint32_t whiteTextureData = 0xffffffff;
		s_Data.PipelineLibrary = MEM::Ref<PipelineLibrary>::Create();
		s_Data.WhiteTexture = MEM::Ref<VulkanTexture>::Create(&whiteTextureData, 1, 1);

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
		s_RenderAPI->Destroy();
		delete s_RenderAPI;
		s_RenderAPI = nullptr;

		auto device = Application::Get()->GetVulkanContext().GetDevice();

		s_Data.GlobalDescriptorManager = nullptr;
		s_Data.PipelineLibrary = nullptr;
		s_Data.WhiteTexture = nullptr;
	}

	void Renderer::BeginFrame() {
		s_RenderAPI->BeginFrame();
	}

	void Renderer::EndFrame() {
		s_RenderAPI->EndFrame();
	}

	void Renderer::DrawMesh(MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform) {
		s_RenderAPI->DrawMesh(pipeline, model, transform);
	}
}