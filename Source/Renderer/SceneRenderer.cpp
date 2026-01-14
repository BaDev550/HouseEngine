#include "hepch.h"
#include "SceneRenderer.h"
#include "Core/Application.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

SceneRenderer::SceneRenderer()
{
	// Camera Uniform buffer set
	VkDeviceSize camerabufferSize = sizeof(CameraUniformData);
	_CameraUB = MEM::MakeScope<VulkanBuffer>(
		camerabufferSize,
		1,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	_CameraUB->Map();

	VkDescriptorBufferInfo cameraBufferInfo = _CameraUB->DescriptorInfo();
	VulkanDescriptorWriter(*Renderer::GetGlobalDescriptorLayout(), *Renderer::GetDescriptorPool())
		.WriteBuffer(0, &cameraBufferInfo)
		.Build(_CameraDS);

	_ImGuiLayer = MEM::MakeScope<ImGuiLayer>();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::DrawScene(std::vector<MEM::Ref<Object>>& objects, MEM::Ref<Camera>& cam)
{
	VkCommandBuffer cmd = Renderer::BeginFrame();
	VulkanCommands::BeginSwapchainRenderPass(cmd);
	_ImGuiLayer->BeginFrame();

	_CameraUD.View = cam->GetView();
	_CameraUD.Proj = cam->GetProjection();
	_CameraUB->WriteToBuffer(&_CameraUD);
	vkCmdBindDescriptorSets(
		cmd,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		Renderer::GetPipelineLayout(),
		0, 1,
		&_CameraDS,
		0,
		nullptr
	);

	ImGui::Begin("Objects");
	for (auto& object : objects) {
		if (ImGui::Selectable(object->_name.c_str())) {
			_SelectedObject = object.get();
		}
		glm::mat4 transform = object->_transform.Mat4();
		Renderer::RenderMesh(cmd, Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline"), object->_Model, transform);
	}

	if (_SelectedObject) {
		ImGui::DragFloat3("Position", glm::value_ptr(_SelectedObject->_transform.position), 0.1f);
		ImGui::DragFloat3("Rotation", glm::value_ptr(_SelectedObject->_transform.rotation), 0.1f);
		ImGui::DragFloat3("Scale", glm::value_ptr(_SelectedObject->_transform.scale), 0.1f);
	}
	ImGui::Text("draw calls: %d", Renderer::GetDrawCall());
	ImGui::Text("FPS: %d", (int)Application::Get()->GetFPS());
	ImGui::End();

	_ImGuiLayer->EndFrame(cmd);
	VulkanCommands::EndSwapchainRenderPass(cmd);
	Renderer::EndFrame();
}
