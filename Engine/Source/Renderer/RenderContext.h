#pragma once

struct GLFWwindow;
namespace House {
	class RenderContext {
	public:
		virtual ~RenderContext() = default;
		virtual void WaitDeviceIdle() = 0;
		static RenderContext* Create(GLFWwindow* window);
	};
}