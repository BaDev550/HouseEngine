#pragma once
#include "Buffer.h"
#include "Texture.h"
#include "Utilities/Memory.h"

namespace House {
	class RenderPass : public MEM::RefCounted
	{
	public:
		virtual ~RenderPass() = default;
		
		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void SetInput(std::string_view name, MEM::Ref<Buffer>& buffer) = 0;
		virtual void SetInput(std::string_view name, MEM::Ref<Texture2D>& texture) = 0;

		static MEM::Ref<RenderPass> Create(MEM::Ref<Pipeline>& pipeline);
	};
}