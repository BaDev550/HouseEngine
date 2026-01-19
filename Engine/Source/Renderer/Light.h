#pragma once
#include <map>
#include <glm/glm.hpp>

namespace House {
	struct alignas(16) DirectionalLight {
		glm::vec3 Direction;
		float pad{ 0.0f };
		glm::vec3 Color;
		float Intensity;

		DirectionalLight() : Direction(0.0f), Color(1.0f), Intensity(1.0f) {}
	};

	struct alignas(16) PointLight {
		glm::vec3 Position;
		float pad{ 0.0f };
		glm::vec3 Color;
		float Intensity;
		float Radius;
		float pad1[3];

		PointLight() : Position(0.0f), Color(1.0f), Intensity(1.0f), Radius(10.0f) {}
	};

	struct UniformBufferDirectionalLight {
		DirectionalLight Light;
	};

	struct UniformBufferPointLights {
		uint32_t Count{ 0 };
		float pad[3];
		PointLight PointLights[1024];
	};
}