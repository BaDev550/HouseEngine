#pragma once

#include "Camera.h"

namespace House {
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;

		virtual void Update(float dt) override;
		void SetProccessingMouse(bool processing) { _proccessingMouse = processing; }
	private:
		float _lastX = 0.0f;
		float _lastY = 0.0f;
		bool _proccessingMouse = true;
	};
}