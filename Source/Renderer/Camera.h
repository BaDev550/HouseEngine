#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	void Update();

	const glm::mat4& GetView() const { return _ViewMatrix; }
	const glm::mat4& GetProjection() const { return _ProjectionMatrix; }
private:
	glm::mat4 _ViewMatrix;
	glm::mat4 _ProjectionMatrix;
	glm::vec3 _Position = glm::vec3(0);
	glm::vec3 _Direction = glm::vec3(0);
	glm::vec3 _Up = glm::vec3(0.0, -1.0f, 0.0f);
	friend class Application; // TEMP
};
