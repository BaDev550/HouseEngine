#pragma once
#include <iostream>
#include "Utilities/Memory.h"
#include "Utilities/UUID.h"
#include "Utilities/Logger.h"
#include "Renderer/Camera.h"
#include "Renderer/Model.h"
#include "Renderer/Light.h"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace House {
	struct IdentityComponent {
		std::string Name;
		UUID UniqeId;

		IdentityComponent(const std::string& name = "UNDEFINED") : Name(name) {}
		IdentityComponent(const std::string& name, UUID id) : Name(name), UniqeId(id) {}
		IdentityComponent(const IdentityComponent&) = default;
	};

	struct TransformComponent {
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);

		glm::mat4 ModelMatrix() {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, Position);
			model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
			model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
			model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
			model = glm::scale(model, Scale);
			return model;
		}

		TransformComponent(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 rot = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f)) : Position(pos), Rotation(rot), Scale(scale) {}
		TransformComponent(const TransformComponent&) = default;
	};

	struct CameraComponent {
		Camera Handle;
		bool IsActive;

		void Update(float dt) { Handle.Update(dt); }
		Camera& GetCamera() { return Handle; }
		CameraComponent(bool isActive = false) : IsActive(isActive) {}
		CameraComponent(const CameraComponent&) = default;
	};

	struct StaticMeshComponent {
		MEM::Ref<Model> Handle; // TODO - make this asset handle
		StaticMeshComponent(const std::string& path = "Resources/DamagedHelmet/DamagedHelmet.gltf") { Handle = MEM::Ref<Model>::Create(path); }
		StaticMeshComponent(const StaticMeshComponent& other) : Handle(other.Handle) {}
		~StaticMeshComponent() {
			Handle = nullptr;
		}
	};

	struct DirectionalLightComponent {
		DirectionalLight Handle;

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent {
		PointLight Handle;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};
}