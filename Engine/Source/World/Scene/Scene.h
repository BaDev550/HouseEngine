#pragma once
#include "World/Components/Components.h"
#include "World/Entity/Entity.h"

#include "Renderer/Camera.h"
#include "Utilities/Defines.h"

namespace House {
	class Scene : public MEM::RefCounted {
	public:
		Scene(const std::string& name);
		~Scene();

		Entity& CreateEntity(const std::string& name);
		Entity& CreateEntityWithUUID(const std::string& name, UUID id);
		void DestroyEntity(Entity entity);
		void Clear();

		Entity GetEntityByUUID(UUID ID);
		Entity GetEntityByID(entt::entity ID);
		Entity FindEntityByName(std::string_view name);
		Entity GetPrimaryCamera();

		void OnRuntimeStart();
		void OnRumtimeStop();
		void OnRuntimeUpdate(float DeltaTime);
		void OnEditorUpdate(float DeltaTime, const MEM::Ref<Camera>& editorCamera);

		const std::string& GetName() const { return _Name; }
		std::unordered_map<UUID, Entity>& GetEntities() { return _Entities; }
		entt::registry& GetRegistry() { return _Registry; }
	private:
		std::string _Name;
		entt::registry _Registry;
		std::unordered_map<UUID, Entity> _Entities;
	protected:
		friend class Entity;
	};
}
#include "World/Entity/EntityFunctions.h"