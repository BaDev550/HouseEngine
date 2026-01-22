#pragma once
#include "Texture.h"
#include "Pipeline.h"
#include "RenderPass.h"
#include "Buffer.h"
#include "Model.h"
#include "RenderAPI.h"
#include "RenderCommandQueue.h"
#include "ShaderLibrary.h"

#include "Utilities/Memory.h"
#include <functional>
#include <array>
#include <glm/glm.hpp>

namespace House {
	class Renderer
	{
	public:
		static void Init();
		static void Destroy();
		static void CompileShaders();
		static void Submit(std::function<void()> func) { s_RenderCommandQueue.Submit(func); }

		static void BeginFrame();
		static bool EndFrame();
		static void CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size);

		static void WaitAndRender();
		static void DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform);
		static void DrawIndexed(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& vertexBuffer, MEM::Ref<Buffer>& indexBuffer, uint32_t count);
		static void DrawVertex(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Buffer>& buffer, uint32_t count);
		static void DrawFullscreenQuad(MEM::Ref<RenderPass>& renderPass);

		static uint32_t GetFrameIndex();
		static RenderStats GetRenderStats();
		static MEM::Ref<Pipeline>& GetPipeline(const std::string& pipeline);
		static MEM::Ref<ShaderLibrary>& GetShaderLibrary();
		static MEM::Ref<Texture2D>& GetWhiteTexture();

		template<typename T = RenderAPI>
		static T* GetAPI() { return static_cast<T*>(s_RenderAPI); }
	private:
		static RenderAPI* s_RenderAPI;
		static RenderCommandQueue s_RenderCommandQueue;
	};
}