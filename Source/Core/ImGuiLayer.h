#pragma once
#include <imgui.h>

class ImGuiLayer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void BeginFrame();
	void EndFrame(VkCommandBuffer cmd);
};