#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	void Update();

	glm::mat4 GetView() const { return _ViewMatrix; }
	glm::mat4 GetProjection() const { return _ProjectionMatrix; }
private:
	glm::mat4 _ViewMatrix = glm::mat4(1.0f);
	glm::mat4 _ProjectionMatrix = glm::mat4(1.0f);
	glm::vec3 _Position = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 _Direction = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 _Up = glm::vec3(0.0, 1.0f, 0.0f);
	glm::vec3 _Forward = glm::vec3(0.0f, 0.0f, 0.0f);
	friend class Application; // TEMP
};
