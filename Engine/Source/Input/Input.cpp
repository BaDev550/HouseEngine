#include "hepch.h"
#include "Input.h"
#include <cstring>

namespace House {
	bool Input::_KeyStates[GLFW_KEY_LAST] = {};
	bool Input::_PrevKeyStates[GLFW_KEY_LAST] = {};

	bool Input::_MouseButtonStates[GLFW_MOUSE_BUTTON_LAST] = {};
	bool Input::_PrevMouseButtonStates[GLFW_MOUSE_BUTTON_LAST] = {};

	glm::vec2 Input::_MousePos = { 0, 0 };
	glm::vec2 Input::_LastMousePos = { 0, 0 };
	glm::vec2 Input::_MouseDelta = { 0, 0 };

	float Input::_ScrollDelta = 0.0f;

	std::unordered_map<std::string, int> Input::_ActionMap;

	void Input::Init()
	{
		glfwSetScrollCallback(Application::Get()->GetWindow().GetHandle(), ScrollCallback);
		_LastMousePos = _MousePos;
	}

	void Input::Update()
	{
		auto handle = Application::Get()->GetWindow().GetHandle();
		std::memcpy(_PrevKeyStates, _KeyStates, sizeof(_KeyStates));
		std::memcpy(_PrevMouseButtonStates, _MouseButtonStates, sizeof(_MouseButtonStates));

		for (int key = GLFW_KEY_SPACE; key < GLFW_KEY_LAST; ++key)
			_KeyStates[key] = glfwGetKey(handle, key) == GLFW_PRESS;

		for (int button = 0; button < GLFW_MOUSE_BUTTON_LAST; ++button)
			_MouseButtonStates[button] = glfwGetMouseButton(handle, button) == GLFW_PRESS;

		_LastMousePos = _MousePos;
		double x, y;
		glfwGetCursorPos(handle, &x, &y);
		_MousePos = { (float)x, (float)y };
		_MouseDelta = _MousePos - _LastMousePos;

		_ScrollDelta = 0.0f;
	}

	bool Input::IsKeyPressed(KeyCode key) { return _KeyStates[key]; }
	bool Input::IsKeyJustPressed(KeyCode key) { return _KeyStates[key] && !_PrevKeyStates[key]; }
	bool Input::IsKeyJustReleased(KeyCode key) { return !_KeyStates[key] && _PrevKeyStates[key]; }

	bool Input::IsMouseButtonPressed(MouseCode button) { return _MouseButtonStates[button]; }
	bool Input::IsMouseButtonJustPressed(MouseCode button) { return _MouseButtonStates[button] && !_PrevMouseButtonStates[button]; }
	bool Input::IsMouseButtonJustReleased(MouseCode button) { return !_MouseButtonStates[button] && _PrevMouseButtonStates[button]; }

	glm::vec2 Input::GetMousePosition() {
		if (!Application::Get()->GetWindow().GetHandle())
			return glm::vec2(0.0);
		return _MousePos;
	}
	glm::vec2 Input::GetMouseDelta() { return _MouseDelta; }
	float Input::GetMouseX() { return _MousePos.x; }
	float Input::GetMouseY() { return _MousePos.y; }

	float Input::GetScrollDelta() { return _ScrollDelta; }

	void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		_ScrollDelta = (float)yoffset;
	}

	void Input::BindAction(const std::string& name, KeyCode key)
	{
		_ActionMap[name] = key;
	}

	bool Input::IsActionPressed(const std::string& name)
	{
		auto it = _ActionMap.find(name);
		if (it == _ActionMap.end()) return false;
		return IsKeyPressed(it->second);
	}
}