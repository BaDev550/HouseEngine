#pragma once
#include "Utilities/Memory.h"
#include "Vulkan/VulkanBuffer.h"
#include "Pipeline.h"
#include "Buffer.h"
#include "Model.h"
#include <glm/glm.hpp>

namespace House {
	enum class GrapichsAPI {
		Vulkan,
		OpenGL
	};

	struct RenderStats {
		float FrameTime;
		uint32_t DrawCall;
		uint32_t TriangleCount;
	};

	class RenderAPI {
	public:
		virtual ~RenderAPI() = default;
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void BeginFrame() = 0;
		virtual bool EndFrame() = 0;

		virtual void CopyBuffer(MEM::Ref<Buffer>& srcBuffer, MEM::Ref<Buffer>& dstBuffer, uint64_t size) = 0;
		virtual void DrawMesh(MEM::Ref<RenderPass>& renderPass, MEM::Ref<Model>& model, glm::mat4& transform) = 0;
		virtual void DrawFullscreenQuad(MEM::Ref<RenderPass>& renderPass) = 0;
		virtual void ResetRenderState() = 0;
		virtual bool FrameStarted() const = 0;
		virtual RenderStats GetRenderStats() = 0;

		static GrapichsAPI CurrentAPI() { return s_GrapichsAPI; }
		static void SetAPI(GrapichsAPI api) { s_GrapichsAPI = api; }
	private:
		inline static GrapichsAPI s_GrapichsAPI = GrapichsAPI::Vulkan;
	};
}