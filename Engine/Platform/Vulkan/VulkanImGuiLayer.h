#pragma once
#include <imgui.h>
#include "Layers/ImGuiLayer.h"
#include "VulkanDescriptor.h"

namespace House {
	class VulkanImGuiLayer : public ImGuiLayer {
	public:
		VulkanImGuiLayer();
		~VulkanImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void Begin() override;
		virtual void End() override;
	private:
		MEM::Ref<VulkanDescriptorPool> _DescriptorPool;
	};
}