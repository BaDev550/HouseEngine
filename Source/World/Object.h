#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Renderer/Model.h"

using entity_id = uint64_t;
struct TransformComponent {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	glm::mat4 Mat4() {
		glm::mat4 transform = glm::mat4(1.0f);
		transform = glm::translate(transform, position);
		transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1, 0, 0));
		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0, 1, 0));
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		transform = glm::scale(transform, scale);
		return transform;
	}
};
class Object {
public:
	entity_id _id = UINT64_MAX;
	std::string _name = "EMPTY_OBJECT";
	TransformComponent _transform;
	MEM::Ref<Model> _Model = nullptr;

	static MEM::Ref<Object> Create(const std::string& name, const std::string& path) {
		static entity_id id = 0;
		MEM::Ref<Object> obj = MEM::MakeRef<Object>(id, path);
		obj->_name = name;
		id++;
		return obj;
	}

	Object(entity_id id, const std::string& path) {
		_id = id;
		if (path != "")
			_Model = MEM::MakeRef<Model>(path);
	}
};