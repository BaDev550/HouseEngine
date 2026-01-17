#pragma once
#include <imgui.h>
#include "Layer.h"

namespace House {
	class ImGuiLayer : public Layer {
	public:
		ImGuiLayer(const char* Name) : Layer(Name) {}
		virtual ~ImGuiLayer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		static ImGuiLayer* Create();
	};
}