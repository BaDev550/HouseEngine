#include <iostream>
#include "Renderer/EditorCamera.h"
#include "Core/Application.h"
#include "World/Scene/Scene.h"
#include "World/Entity/Entity.h"

class HouseEditorLayer : public Layer {
public:
	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender() override;
private:
	MEM::Scope<Scene> _ActiveScene;
	MEM::Scope<SceneRenderer> _SceneRenderer;
	MEM::Ref<EditorCamera> _EditorCamera;
	Entity* _SelectedEntity = nullptr;
};