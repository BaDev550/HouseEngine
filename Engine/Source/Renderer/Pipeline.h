#pragma once
#include "Utilities/Memory.h"

#include "World/Entity/Entity.h"
#include "World/Scene/Scene.h"
#include "Camera.h"
#include <array>

namespace House {
	class Pipeline : public MEM::RefCounted
	{
	public:
		virtual ~Pipeline() = default;
		
		virtual void Invalidate() = 0;
		virtual MEM::Ref<> GetShader() const = 0;

		static MEM::Ref<Pipeline> Create();
	};
}