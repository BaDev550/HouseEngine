#include "hepch.h"
#include "Renderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>

#include "Vulkan/VulkanRenderAPI.h"

namespace House {
	struct RenderData {
		MEM::Ref<ShaderLibrary> ShaderLibrary = nullptr;
		MEM::Ref<Texture2D> WhiteTexture = nullptr;

		std::map<std::string, MEM::Ref<Pipeline>> CompiledPipelines;
	} s_Data;

	RenderAPI* Renderer::s_RenderAPI = nullptr;
	RenderCommandQueue Renderer::s_RenderCommandQueue;
	RenderAPI* CreateRenderAPI() {
		switch (RenderAPI::CurrentAPI())
		{
		case GrapichsAPI::Vulkan: return new VulkanRenderAPI();
		case GrapichsAPI::OpenGL: return nullptr;
		}
	}

	uint32_t Renderer::GetFrameIndex() { return Application::Get()->GetFrameIndex(); }
	RenderStats Renderer::GetRenderStats() { return s_RenderAPI->GetRenderStats(); }
	MEM::Ref<ShaderLibrary>& Renderer::GetShaderLibrary() { return s_Data.ShaderLibrary; }

	MEM::Ref<Pipeline>& Renderer::GetPipeline(const std::string& pipeline) { 
		return s_Data.CompiledPipelines.at(pipeline);
	}

	MEM::Ref<Texture2D>& Renderer::GetWhiteTexture() { return s_Data.WhiteTexture; }

	void Renderer::Init() {
		s_RenderAPI = CreateRenderAPI();
		s_RenderAPI->Init();

		TextureSpecification whiteTextureSpec{};
		whiteTextureSpec.Width = 1;
		whiteTextureSpec.Height = 1;
		whiteTextureSpec.MipLevels = false;
		uint32_t whiteTextureData = 0xffffffff;
		s_Data.WhiteTexture = Texture2D::Create(whiteTextureSpec, DataBuffer(&whiteTextureData, sizeof(uint32_t)));

		s_Data.ShaderLibrary = MEM::Ref<ShaderLibrary>::Create();

		Renderer::GetShaderLibrary()->Load("PBRStatic", "Shaders/pbrstatic.vert", "Shaders/pbrstatic.frag");
		Renderer::GetShaderLibrary()->Load("DeferredLighting", "Shaders/deferredLighting.vert", "Shaders/deferredLighting.frag");
		Renderer::GetShaderLibrary()->Load("Grid", "Shaders/grid.vert", "Shaders/grid.frag");
		Renderer::GetShaderLibrary()->Load("SSAO", "Shaders/SSAO.vert", "Shaders/SSAO.frag");
		Renderer::GetShaderLibrary()->Load("SSAO_Blur", "Shaders/SSAO_Blur.vert", "Shaders/SSAO_Blur.frag");
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
			s_Data.CompiledPipelines[name] = Pipeline::Create(data);
		}
	}

	void Renderer::BeginFrame() {
		s_RenderAPI->BeginFrame();
	}

	bool Renderer::EndFrame() {
		return s_RenderAPI->EndFrame();
	}

	void Renderer::CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size) {
		s_RenderAPI->CopyBuffer(srcBuffer, dstBuffer, size);
	}

	void Renderer::WaitAndRender()
	{
		s_RenderCommandQueue.Execute();
	}

	void Renderer::DrawStaticMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<StaticMesh>& mesh, MEM::Ref<MeshSource>& meshSource, glm::mat4& transform) {
		s_RenderAPI->DrawStaticMesh(renderPass, mesh, meshSource, transform);
	}
	void Renderer::DrawIndexed(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& vertexBuffer, MEM::Ref<Buffer>& indexBuffer, uint32_t count)
	{
		s_RenderAPI->DrawIndexed(pipeline, vertexBuffer, indexBuffer, count);
	}
	void Renderer::DrawVertex(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& buffer, uint32_t count)
	{
		s_RenderAPI->DrawVertex(pipeline, buffer, count);
	}
	void Renderer::DrawFullscreenQuad(MEM::Ref<RenderPass>& renderPass)
	{
		s_RenderAPI->DrawFullscreenQuad(renderPass);
	}
}