#include "HouseEditor.h"
#include "Utilities/Platform.h"
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"
#include "World/Scene/SceneSerializer.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <backends/imgui_impl_vulkan.h>
#include "Vulkan/VulkanTexture.h"

namespace House::Editor {
	HouseEditorLayer::HouseEditorLayer()
		: Layer("EditorLayer")
	{
	}

	void HouseEditorLayer::OnAttach()
	{
		auto commandLineArgs = Application::Get()->GetApplicationSpecs().CommandLineArgs;
		if (commandLineArgs.Count > 1) {
			auto projectPath = std::filesystem::path(commandLineArgs[1]);
			OpenProject(projectPath);
		}
		else {
			std::string projectPath = Platform::FileDialog::OpenFile(".hproj");
			OpenProject(projectPath);
		}

		_EditorCamera = MEM::Ref<EditorCamera>::Create();
		_SceneRenderer = MEM::Ref<SceneRenderer>::Create(_ActiveScene);

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
		static bool cursor = true;

		if (Input::IsKeyJustPressed(Key::F1)) {
			cursor = !cursor;
			_EditorCamera->SetFirstMouse();
			Application::Get()->GetWindow().EnableCursor(cursor);
		}
		if (Input::IsKeyJustPressed(Key::F3)) {
			SaveScene();
		}
		if (Input::IsKeyJustPressed(Key::F2)) {
			OpenScene("F:/Github/HouseEngine/NewProject/Assets/New Scene.hscene");
		}

		_ActiveScene->OnEditorUpdate(dt, _SceneRenderer, *_EditorCamera);
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
			if (ImGui::Button("Add Sun Light Entity")) {
				auto entity = _ActiveScene->CreateEntity("SUN_LIGHT");
				entity.AddComponent<DirectionalLightComponent>();
			}
			if (ImGui::Button("Add Point Light Entity")) {
				auto entity = _ActiveScene->CreateEntity("POINT_LIGHT");
				entity.AddComponent<PointLightComponent>();
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

			ImGui::Image(gbuffer->GetAttachmentTexture(0)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Image(gbuffer->GetAttachmentTexture(1)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
			ImGui::Image(gbuffer->GetAttachmentTexture(2)->GetImGuiTextureID(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}
		if (ImGui::CollapsingHeader("Light Data")) {
			auto dirLights = _ActiveScene->GetRegistry().view<DirectionalLightComponent>();
			for (auto entity : dirLights) {
				auto& dLight = dirLights.get<DirectionalLightComponent>(entity);
				ImGui::Text("SunLight");
				ImGui::DragFloat("Intensity##Sun", &dLight.Handle.Intensity, 0.1f, 0.0f, 100.0f);
				ImGui::ColorEdit3("Color##Sun", glm::value_ptr(dLight.Handle.Color), 0.1f);
			}

			ImGui::Separator();

			ImGui::Text("Point Lights");
			auto pointLights = _ActiveScene->GetRegistry().view<PointLightComponent, TransformComponent>();
			int i = 0;
			for (auto entity : pointLights) {
				auto& pLight = pointLights.get<PointLightComponent>(entity);

				std::string label = "Point Light " + std::to_string(i++);
				if (ImGui::TreeNode(label.c_str())) {
					ImGui::DragFloat("Intensity", &pLight.Handle.Intensity, 0.1f, 0.0f, 100.0f);
					ImGui::DragFloat("Radius", &pLight.Handle.Radius, 0.1f, 0.0f, 100.0f);
					ImGui::ColorEdit3("Color", glm::value_ptr(pLight.Handle.Color));
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();

		ImGui::Begin("Engine Editor");

		if (ImGui::CollapsingHeader("Assets")) {
			std::filesystem::path projectDir = Project::GetProjectDirectory();
			std::filesystem::path assetDir = Project::GetAssetDirectory();
			ImGui::Text("Project Directory: %s", projectDir.string().c_str());
			ImGui::Text("Asset Directory: %s", assetDir.string().c_str());

			for (auto& dirEntry : std::filesystem::directory_iterator(assetDir)) {
				if (dirEntry.is_directory()) {
					std::string dirName = dirEntry.path().filename().string();
					if (ImGui::TreeNode(dirName.c_str())) {
						for (auto& fileEntry : std::filesystem::directory_iterator(dirEntry.path())) {
							std::string fileName = fileEntry.path().filename().string();
							ImGui::Text(fileName.c_str());
						}
						ImGui::TreePop();
					}
				}
				else {
					std::string fileName = dirEntry.path().filename().string();
					if (ImGui::Selectable(fileName.c_str())) {
						std::filesystem::path selectedFilePath = dirEntry.path();
						if (selectedFilePath.extension() == ".hscene") {
							OpenScene(dirEntry.path());
						}
					}
				}
			}
		}

		ImGui::End();
	}

	void HouseEditorLayer::NewScene()
	{
		_ActiveScene = MEM::Ref<Scene>::Create("New Scene");
	}

	void HouseEditorLayer::OpenScene(const std::filesystem::path& path)
	{
		Renderer::GetAPI()->ResetRenderState();

		MEM::Ref<Scene> newScene = MEM::Ref<Scene>::Create("New Scene");
		SceneSerializer serializer(newScene);
		serializer.Deserialize(path);

		_ActiveScene->Clear();
		_ActiveScene = newScene;
		_SceneRenderer->SetScene(_ActiveScene);
	}

	void HouseEditorLayer::SaveSceneAs()
	{
	}

	void HouseEditorLayer::SaveScene()
	{
		SceneSerializer serializer(_ActiveScene);
		std::filesystem::path scenePath = Project::GetAssetDirectory() / (_ActiveScene->GetName() + ".hscene");
		serializer.Serialize(scenePath);
	}

	void HouseEditorLayer::NewProject()
	{
		Project::New();
	}

	void HouseEditorLayer::OpenProject(const std::filesystem::path& path)
	{
		if (Project::Load(path)) {
			_ActiveScene = MEM::Ref<Scene>::Create("New Scene");
		};
	}

	void HouseEditorLayer::SaveProject()
	{
	}
}