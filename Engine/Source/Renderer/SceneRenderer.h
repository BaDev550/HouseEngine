#pragma once
#include "Utilities/Memory.h"

#include "World/Entity/Entity.h"
#include "World/Scene/Scene.h"
#include "RenderPass.h"
#include "Camera.h"
#include "Buffer.h"
#include <array>

namespace House {
	class SceneRenderer : public MEM::RefCounted
	{
	public:
		SceneRenderer(MEM::Ref<Scene>& scene);
		~SceneRenderer();

		void DrawScene(const MEM::Ref<Camera>& cam);
	private:
		struct CameraUniformData {
			glm::mat4 View;
			glm::mat4 Proj;
		} _CameraUD;

		MEM::Ref<Buffer> _CameraUB;
		MEM::Ref<RenderPass> _MainRenderPass;

		MEM::Ref<Scene> _Scene;
	};
}