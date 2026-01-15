#include "HouseEditor.h"
#include <glm/gtc/type_ptr.hpp>

void HouseEditorLayer::OnAttach()
{
	_ActiveScene = MEM::MakeScope<Scene>("Test Scene");
	_SceneRenderer = MEM::MakeScope<SceneRenderer>();
	_EditorCamera = MEM::Ref<EditorCamera>::Create();

	for (int i = 0; i < 10; i++) {
		auto entity = _ActiveScene->CreateEntity("NEW_ENTITY");
		entity.GetComponent<TransformComponent>().Position = glm::vec3(i * 3.0f, 0.0f, 0.0f);
		entity.AddComponent<StaticMeshComponent>();
	}
}

void HouseEditorLayer::OnDetach()
{
}

void HouseEditorLayer::OnUpdate(float dt)
{
	_EditorCamera->Update(dt);
	_SceneRenderer->DrawScene(_ActiveScene->GetEntities(), _EditorCamera);
}

void HouseEditorLayer::OnImGuiRender()
{
	std::string debuggerPanelName = "Scene: " + _ActiveScene->GetName() + " Debug Panel";
	ImGui::Begin(debuggerPanelName.c_str());

	if (ImGui::CollapsingHeader("Entities")) {
		if (ImGui::Button("Add Entity")) {
			auto entity = _ActiveScene->CreateEntity("NEW_ENTITY");
			entity.AddComponent<StaticMeshComponent>();
		}

		for (auto& [id, entity] : _ActiveScene->GetEntities()) {
			std::string uniqeEntityName = entity.GetName() + "##" + entity.GetUUID().ToString();
			if (ImGui::Selectable(uniqeEntityName.c_str()))
				_SelectedEntity = &entity;
		}

		if (_SelectedEntity) {
			auto& ic = _SelectedEntity->GetComponent<IdentityComponent>();
			auto& tc = _SelectedEntity->GetComponent<TransformComponent>();
			ImGui::Text(ic.Name.c_str());
			ImGui::Text(ic.UniqeId.ToString().c_str());
			ImGui::DragFloat3("Position", glm::value_ptr(tc.Position), 0.1f);
			ImGui::DragFloat3("Rotation", glm::value_ptr(tc.Rotation), 0.1f);
			ImGui::DragFloat3("Scale", glm::value_ptr(tc.Scale), 0.1f);
		}
	}
	if (ImGui::CollapsingHeader("Camera Data")) {
		ImGui::Text("Position: %s", glm::to_string(_EditorCamera->GetPosition()).c_str());
	}
	if (ImGui::CollapsingHeader("Render Data")) {
		ImGui::Text("FPS: %d", (int)Application::Get()->GetFPS());
		ImGui::Text("Draw Calls: %d", Renderer::GetDrawCall());
	}
	ImGui::End();
}