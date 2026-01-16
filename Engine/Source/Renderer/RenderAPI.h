#pragma once
#include "Utilities/Memory.h"
#include "Vulkan/VulkanBuffer.h"
#include "Vulkan/VulkanPipeline.h"
#include "Model.h"
#include <glm/glm.hpp>

namespace House {
	enum class GrapichsAPI {
		Vulkan,
		OpenGL
	};

	class RenderAPI {
	public:
		virtual ~RenderAPI() = default;
		virtual void Init() = 0;
		virtual void Destroy() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void DrawMesh(MEM::Ref<VulkanPipeline>& pipeline, MEM::Ref<Model>& model, glm::mat4& transform) = 0;
		virtual void DrawVertex(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, uint32_t vertexCount) = 0;
		virtual void DrawIndexed(MEM::Ref<VulkanPipeline>& pipeline, const MEM::Ref<VulkanBuffer>& vertexBuffer, const MEM::Ref<VulkanBuffer>& indexBuffer, uint32_t indicesCount) = 0;
		virtual uint32_t GetDrawCall() = 0;

		static GrapichsAPI CurrentAPI() { return s_GrapichsAPI; }
		static void SetAPI(GrapichsAPI api) { s_GrapichsAPI = api; }
	private:
		inline static GrapichsAPI s_GrapichsAPI = GrapichsAPI::Vulkan;
	};
}