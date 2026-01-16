#include "hepch.h"
#include "Renderer.h"
#include "RenderAPI.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

#include "Vulkan/VulkanRenderAPI.h"

namespace House {
	struct RenderData {
		MEM::Ref<ShaderLibrary> ShaderLibrary = nullptr;
		MEM::Ref<Texture2D> WhiteTexture = nullptr;
		MEM::Ref<Texture2D> BlackTexture = nullptr;

		std::map<std::string, MEM::Ref<Pipeline>> CompiledPipelines;
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
	uint32_t Renderer::GetFrameIndex() { return Application::Get()->GetFrameIndex(); }

	MEM::Ref<ShaderLibrary>& Renderer::GetShaderLibrary() { return s_Data.ShaderLibrary; }
	MEM::Ref<Pipeline>& Renderer::GetPipeline(const std::string& pipeline) { s_Data.CompiledPipelines[pipeline]; }
	MEM::Ref<Texture2D>& Renderer::GetWhiteTexture() { return s_Data.WhiteTexture; }

	void Renderer::Init() {
		s_RenderAPI = CreateRenderAPI();
		s_RenderAPI->Init();

		uint32_t whiteTextureData = 0xffffffff;
		uint32_t blackTextureData = 0x00000000;
		s_Data.ShaderLibrary = MEM::Ref<ShaderLibrary>::Create();

		s_Data.WhiteTexture = Texture2D::Create(&whiteTextureData, 1, 1);
		s_Data.BlackTexture = Texture2D::Create(&blackTextureData, 1, 1);

		Renderer::GetShaderLibrary()->Load("MainShader", "Shaders/base.vert", "Shaders/base.frag");
		Renderer::CompileShaders();
	}

	void Renderer::Destroy()
	{
		s_RenderAPI->Destroy();
		delete s_RenderAPI;
		s_RenderAPI = nullptr;

		s_Data.CompiledPipelines.clear();
		s_Data.ShaderLibrary = nullptr;
		s_Data.WhiteTexture = nullptr;
	}

	void Renderer::CompileShaders()
	{
		for (auto& [name, shader] : Renderer::GetShaderLibrary()->GetShaders()) {
			PipelineData data{};
			data.Shader = shader;
			data.RenderPass = Application::Get()->GetWindow().GetSwapchain().GetRenderPass();
			s_Data.CompiledPipelines[name] = Pipeline::Create(data);
		}
	}

	void Renderer::BeginFrame() {
		s_RenderAPI->BeginFrame();
	}

	void Renderer::EndFrame() {
		s_RenderAPI->EndFrame();
	}

	void Renderer::CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size) {
		s_RenderAPI->CopyBuffer(srcBuffer, dstBuffer, size);
	}

	void Renderer::DrawMesh(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform) {
		s_RenderAPI->DrawMesh(pipeline, model, transform);
	}
}