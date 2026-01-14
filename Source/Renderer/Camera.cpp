#include "hepch.h"
#include "Camera.h"

void Camera::Update()
{
	_Forward.x = cosf(_Direction.x) * sinf(_Direction.y);
	_Forward.y = sinf(_Direction.x);
	_Forward.z = -cosf(_Direction.x) * cosf(_Direction.y);
	_Forward = glm::normalize(_Forward);

	float width = (float)Application::Get()->GetWindow().GetWidth();
	float height = (float)Application::Get()->GetWindow().GetHeight();
	float aspect = width / height;

	const float c3 = glm::cos(_Direction.z);
	const float s3 = glm::sin(_Direction.z);
	const float c2 = glm::cos(_Direction.x);
	const float s2 = glm::sin(_Direction.x);
	const float c1 = glm::cos(_Direction.y);
	const float s1 = glm::sin(_Direction.y);
	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
	_ViewMatrix = glm::mat4{ 1.f };
	_ViewMatrix[0][0] = u.x;
	_ViewMatrix[1][0] = u.y;
	_ViewMatrix[2][0] = u.z;
	_ViewMatrix[0][1] = v.x;
	_ViewMatrix[1][1] = v.y;
	_ViewMatrix[2][1] = v.z;
	_ViewMatrix[0][2] = w.x;
	_ViewMatrix[1][2] = w.y;
	_ViewMatrix[2][2] = w.z;
	_ViewMatrix[3][0] = -glm::dot(u, _Position);
	_ViewMatrix[3][1] = -glm::dot(v, _Position);
	_ViewMatrix[3][2] = -glm::dot(w, _Position);

	const float tanHalfFOV = tan(45.0 / 2.0f);
	_ProjectionMatrix = glm::mat4{ 0.0f };
	_ProjectionMatrix[0][0] = 1.0f / (aspect * tanHalfFOV);
	_ProjectionMatrix[1][1] = 1.0f / (tanHalfFOV);
	_ProjectionMatrix[1][1] *= -1.0f;
	_ProjectionMatrix[2][2] = 100.0f / (100.0f - 0.1f);
	_ProjectionMatrix[2][3] = 1.0f;
	_ProjectionMatrix[3][2] = -(100.0f * 0.1f) / (100.0f - 0.1f);
}
