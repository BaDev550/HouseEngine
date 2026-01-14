#include "hepch.h"
#include "Camera.h"

void Camera::Orbit(float pitchOffset, float yawOffset)
{
	AddPitch(pitchOffset);
	AddYaw(yawOffset);
}

void Camera::CalculateCameraMatrixes() {
	_Forward.x = cosf(_Direction.x) * sinf(_Direction.y);
	_Forward.y = sinf(_Direction.x);
	_Forward.z = -cosf(_Direction.x) * cosf(_Direction.y);
	_Forward = glm::normalize(_Forward);

	float width = (float)Application::Get()->GetWindow().GetWidth();
	float height = (float)Application::Get()->GetWindow().GetHeight();
	float aspect = width / height;

	glm::vec3 worldUp = glm::vec3(0.0f, -1.0f, 0.0f);
	if (glm::abs(glm::dot(_Forward, worldUp)) > 0.999f) { _Right = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), _Forward)); }
	else { _Right = glm::normalize(glm::cross(_Forward, worldUp)); }

	_ViewMatrix = glm::lookAt(_Position, _Position + _Forward, _Up);
	_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
}

void Camera::AddPitch(float value)
{
	_Direction.x += value;
}

void Camera::AddYaw(float value)
{
	_Direction.y += value;
}
