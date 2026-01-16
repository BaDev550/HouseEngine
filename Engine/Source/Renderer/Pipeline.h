#pragma once
#include "Utilities/Memory.h"
#include "Shader.h"

namespace House {
	struct PipelineData {
		MEM::Ref<Shader> Shader;
		VkRenderPass RenderPass;
		//MEM::Ref<Framebuffer> Framebuffer;
	};
	
	class Pipeline : public MEM::RefCounted
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineData& GetPipelineData() = 0;
		virtual const PipelineData& GetPipelineData() const = 0;

		virtual void Invalidate() = 0;
		virtual MEM::Ref<Shader> GetShader() const = 0;

		static MEM::Ref<Pipeline> Create(const PipelineData& data);
	};
}