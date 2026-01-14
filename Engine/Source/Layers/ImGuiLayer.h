#pragma once
#include <imgui.h>
#include "Layer.h"

class ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	void Begin();
	void End(VkCommandBuffer cmd);
};