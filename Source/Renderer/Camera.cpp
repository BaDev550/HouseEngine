#include "hepch.h"
#include "Camera.h"

void Camera::Update()
{
	float width = (float)Application::Get()->GetWindow().GetWidth();
	float height = (float)Application::Get()->GetWindow().GetHeight();
	float aspect = width / height;
	_ViewMatrix = glm::lookAt(_Position, _Position + _Direction, _Up);
	_ProjectionMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	_ProjectionMatrix[1][1] *= -1;
}
