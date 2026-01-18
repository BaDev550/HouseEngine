#include "HouseEditor.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace House::Editor {
	HouseEditorLayer::HouseEditorLayer()
		: Layer("EditorLayer")
	{
	}

	void HouseEditorLayer::OnAttach()
	{
		_ActiveScene = MEM::Ref<Scene>::Create("Test Scene");
		_SceneRenderer = MEM::Ref<SceneRenderer>::Create(_ActiveScene);
		_EditorCamera = MEM::Ref<EditorCamera>::Create();

		for (int i = 0; i < 1; i++) {
			auto entity = _ActiveScene->CreateEntity("NEW_ENTITY");
			entity.GetComponent<TransformComponent>().Position = glm::vec3(i * 3.0f, 0.0f, 0.0f);
			entity.AddComponent<StaticMeshComponent>();
		}
	}

	void HouseEditorLayer::OnDetach()
	{
		_SceneRenderer = nullptr;
		_ActiveScene->Clear();
		_ActiveScene = nullptr;
	}

	void HouseEditorLayer::OnUpdate(float dt)
	{
		_EditorCamera->Update(dt);
		_SceneRenderer->DrawScene(_EditorCamera);
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
			ImGui::Text("Draw Calls: %d", Renderer::GetRenderStats().DrawCall);
			ImGui::Text("Frame Time: %f/ms", Renderer::GetRenderStats().FrameTime);
			ImGui::Text("Trianlge Count: %d", Renderer::GetRenderStats().TriangleCount);
		}
		if (ImGui::CollapsingHeader("Deferred Rendering Debug")) {
			auto gbufferPass = _SceneRenderer->GetGBufferRenderPass();
			auto gbuffer = gbufferPass->GetFramebuffer();
			ImGui::Image((ImTextureID)(void*)gbuffer->GetAttachmentTexture(0)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1,1,1,1), ImVec4(1,0,0,1));
			ImGui::Image((ImTextureID)(void*)gbuffer->GetAttachmentTexture(1)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1,1,1,1), ImVec4(1,0,0,1));
			ImGui::Image((ImTextureID)(void*)gbuffer->GetAttachmentTexture(2)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1,1,1,1), ImVec4(1,0,0,1));
		}
		ImGui::End();
	}
}