#include "hepch.h"
#include "Scene.h"

#include "Renderer/SceneRenderer.h"
#include "World/Entity/Entity.h"

namespace House {
	Scene::Scene(const std::string& name) : _Name(name) {}
	Scene::~Scene() {
		Clear();
	}

	void Scene::Clear() {
		LOG_CORE_INFO("Cleaning up Scene {}", _Name);
		_Entities.clear();
		_Registry.clear();
	}

	Entity& Scene::CreateEntity(const std::string& name) {
		return CreateEntityWithUUID(name, UUID());
	}

	Entity& Scene::CreateEntityWithUUID(const std::string& name, UUID id)
	{
		Entity entity = { _Registry.create(), this };
		entity.AddComponent<IdentityComponent>(name, id);
		entity.AddComponent<TransformComponent>();
		_Entities[id] = entity;
		return _Entities[id];
	}

	void Scene::DestroyEntity(Entity entity) {
		_Entities.erase(entity.GetUUID());
		_Registry.destroy(entity);
	}

	Entity Scene::GetEntityByUUID(UUID ID) { return _Entities[ID]; }
	Entity Scene::GetEntityByID(entt::entity ID) {
		if (_Registry.valid(ID)) {
			UUID id = _Registry.get<IdentityComponent>(ID).UniqeId;
			return Entity{ ID, this };
		}
	}

	Entity Scene::FindEntityByName(std::string_view name) {
		auto view = _Registry.view<IdentityComponent>();
		for (auto& entity : view) {
			const auto& ic = view.get<IdentityComponent>(entity);
			if (ic.Name == name)
				return Entity{ entity, this };
		}
	}

	Entity Scene::GetPrimaryCamera() {
		auto view = _Registry.view<IdentityComponent, CameraComponent>();
		for (auto& entity : view) {
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.IsActive)
				return GetEntityByID(entity);
		}
	}

	void Scene::OnRuntimeStart() {}
	void Scene::OnRumtimeStop() {}

	void Scene::OnRuntimeUpdate(float DeltaTime, MEM::Ref<SceneRenderer>& renderer) {
		if (GetPrimaryCamera()) {
			auto mainCameraComponent = GetPrimaryCamera().GetComponent<CameraComponent>();
			renderer->DrawScene(mainCameraComponent.GetCamera());
		}
	}

	void Scene::OnEditorUpdate(float DeltaTime, MEM::Ref<SceneRenderer>& renderer, Camera& editorCamera) {
		renderer->DrawScene(editorCamera);
	}
}