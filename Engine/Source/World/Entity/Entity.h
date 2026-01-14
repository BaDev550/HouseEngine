#pragma once
#include <iostream>
#include "Utilities/UUID.h"
#include <entt/entt.hpp>

class Scene;
class Entity {
public:
	Entity() = default;
	Entity(entt::entity handle, Scene* scene) : _Handle(handle), _Scene(scene) {}
	Entity(const Entity& other) = delete;
	Entity& operator=(Entity&) = delete;
private:
	UUID _Handle;
	Scene* _Scene = nullptr;
};