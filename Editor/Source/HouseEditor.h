#include <iostream>
#include "Renderer/EditorCamera.h"
#include "Renderer/SceneRenderer.h"
#include "Core/Application.h"
#include "World/Scene/Scene.h"
#include "World/Entity/Entity.h"

namespace House::Editor {
	enum class EditorState {
		Edit, 
		Play, 
		Pause, 
		Simulate
	};
	class HouseEditorLayer : public Layer {
	public:
		HouseEditorLayer();
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnImGuiRender() override;
	private:
		void NewScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();
		void SaveScene();

		void NewProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();
	private:
		MEM::Ref<Scene> _ActiveScene;
		MEM::Ref<Scene> _EditorScene;
		MEM::Ref<SceneRenderer> _SceneRenderer;
		MEM::Ref<EditorCamera> _EditorCamera;
		Entity* _SelectedEntity = nullptr;

		EditorState _EditorState = EditorState::Edit;

		bool _PendingSceneLoad = false;
		std::filesystem::path _PendingScenePath;
	};
}