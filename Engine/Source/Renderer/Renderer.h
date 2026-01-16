#pragma once
#include "Texture.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Model.h"
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

		static void BeginFrame();
		static void EndFrame();
		static void CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size);
		static void DrawMesh(MEM::Ref<Pipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform);

		static uint32_t GetDrawCall();
		static uint32_t GetFrameIndex();
		static MEM::Ref<Pipeline>& GetPipeline(const std::string& pipeline);
		static MEM::Ref<ShaderLibrary>& GetShaderLibrary();
		static MEM::Ref<Texture2D>& GetWhiteTexture();
	};
}