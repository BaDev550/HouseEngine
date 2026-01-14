#include "hepch.h"
#include "Scene.h"

#include "World/Entity/Entity.h"

Scene::Scene(const std::string& name) 
	: _Name(name)
{

}

Scene::~Scene() {
	_Entities.clear();
	_Registry.clear();
}

Entity Scene::CreateEntity(const std::string& name) {
	Entity entity = { _Registry.create(), this };
	UUID entityID = UUID();
	entity.AddComponent<IdentityComponent>(name, entityID);
	entity.AddComponent<TransformComponent>();
	_Entities[entityID] = entity;
	return entity;
}

void Scene::DestroyEntity(Entity entity) {
	_Entities.erase(entity.GetUUID());
	_Registry.destroy(entity);
}

Entity& Scene::GetEntityByUUID(UUID ID) { return _Entities[ID]; }
Entity& Scene::GetEntityByID(entt::entity ID) {
	if (_Registry.valid(ID)) {
		UUID id = _Registry.get<IdentityComponent>(ID).UniqeId;
		return GetEntityByUUID(id);
	}
}

Entity& Scene::FindEntityByName(std::string_view name) {
	auto view = _Registry.view<IdentityComponent>();
	for (auto& entity : view) {
		const auto& ic = view.get<IdentityComponent>(entity);
		if (ic.Name == name)
			return GetEntityByID(entity);
	}
}

Entity& Scene::GetPrimaryCamera() {
	auto view = _Registry.view<IdentityComponent, CameraComponent>();
	for (auto& entity : view) {
		const auto& camera = view.get<CameraComponent>(entity);
		if (camera.IsActive)
			return GetEntityByID(entity);
	}
}

void Scene::OnRuntimeStart() {}
void Scene::OnRumtimeStop() {}

void Scene::OnRuntimeUpdate(float DeltaTime) {
	
}

void Scene::OnEditorUpdate(float DeltaTime, const MEM::Ref<Camera>& editorCamera) {
	
}
