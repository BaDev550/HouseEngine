#pragma once
#include "World/Components/Components.h"
#include "Utilities/Defines.h"
#include <entt/entt.hpp>

namespace House {
	class Scene;
	class Entity {
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene) : _Handle(handle), _Scene(scene) {}
		Entity(const Entity& other) = default;

		template<typename T>
		bool HasComponent();

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args);

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args);

		template<typename T>
		T& GetComponent();

		template<typename T>
		const T& GetComponent() const;

		template<typename T>
		void RemoveComponent();

		UUID GetUUID() { return GetComponent<IdentityComponent>().UniqeId; }
		const std::string& GetName() { return GetComponent<IdentityComponent>().Name; }

		operator entt::entity() const { return _Handle; }
		operator bool() const { return _Handle != entt::null; }
		operator uint64_t() const { return static_cast<uint64_t>(_Handle); }
		bool operator==(const Entity& other) const { return _Handle == other._Handle; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
	private:
		entt::entity _Handle;
		Scene* _Scene = nullptr;
	};
}