#pragma once

namespace House {
	class Layer {
	public:
		Layer(const char* name = "EMPT_LAYER") : _Name(name) {}
		virtual ~Layer() = default;
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnImGuiRender() {}
		inline const char* GetName() const { return _Name; }
	private:
		const char* _Name;
	};
}