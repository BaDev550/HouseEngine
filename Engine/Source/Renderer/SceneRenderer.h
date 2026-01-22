#pragma once
#include "Utilities/Memory.h"

#include "World/Scene/Scene.h"
#include "World/Entity/Entity.h"
#include "RenderPass.h"
#include "Camera.h"
#include "Buffer.h"
#include "Light.h"
#include <array>

namespace House {
	struct SceneLightEnviroment {
		DirectionalLight DirectionalLight;
		std::vector<PointLight> PointLights;
		[[nodiscard]] uint32_t GetPointLightCount() const { return static_cast<uint32_t>(PointLights.size()); }
	};

	struct LightUniformBuffers {
		UniformBufferDirectionalLight UBDDirectionalLight;
		UniformBufferPointLights UBDPointLights;
	};

	struct alignas(16) CameraUniformData {
		glm::mat4 View;
		glm::mat4 Proj;
		glm::vec3 Position;
	};

	struct SceneRenderData {
		CameraUniformData CameraData;
		SceneLightEnviroment LightEnviromentData;
		LightUniformBuffers LightEnviromentUniformData;
	};

	class SceneRenderer : public MEM::RefCounted
	{
	public:
		SceneRenderer(MEM::Ref<Scene>& scene);
		~SceneRenderer();

		MEM::Ref<RenderPass> GetGBufferRenderPass() const { return _GRenderPass; }
		SceneRenderData& GetSceneData() { return _SceneData; }

		void DrawScene(Camera& cam);
		void SetScene(MEM::Ref<Scene>& scene) { _Scene = scene; }
	private:
		void CollectLightDataFromScene();

		MEM::Ref<Scene> _Scene;

		SceneRenderData _SceneData;
		MEM::Ref<Buffer> _CameraUB;
		MEM::Ref<Buffer> _PointLightsUB;
		MEM::Ref<Buffer> _DirectionalLightUB;

		MEM::Ref<Pipeline> _GPipeline;
		MEM::Ref<RenderPass> _GRenderPass;

		MEM::Ref<Pipeline> _FinalImagePipeline;
		MEM::Ref<RenderPass> _FinalImageRenderPass;
	};
}