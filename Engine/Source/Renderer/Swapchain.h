#pragma once
#include <iostream>

namespace House {
	class Swapchain {
	public:
		virtual ~Swapchain() = default;
		virtual bool Swapbuffers(uint32_t* imageIndex = nullptr) = 0;
		static Swapchain* Create(RenderContext* Context);
	};
}