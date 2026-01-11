#pragma once
#include <memory>
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanDescriptor.h"
#include "Vulkan/VulkanBuffer.h"
#include "Renderer.h"
#include "Window.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#define APPLICATIONNAME "Application"

struct Vertex {
	glm::vec2 Position;
	glm::vec3 Color;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);
		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Proj;
};

class Application
{
public:
	Application();
	~Application();

	static Application* Get() { return _Instance; }
	void Run();
public:
	Window* GetWindow() const { return _Window.get(); }
	VulkanContext* GetVulkanContext() const { return _VulkanContext.get(); }
	VkDescriptorSetLayout GetSetLayouts() const { return _DescriptorSetLayout->GetDescriptorSetLayout(); }

	void CreateVertexBuffer();
	void CreateIndexBuffer();
private:
	static Application* _Instance;

	std::unique_ptr<Window> _Window;
	std::unique_ptr<Renderer> _Renderer;
	std::unique_ptr<VulkanContext> _VulkanContext;

	std::unique_ptr<VulkanBuffer> _VertexBuffer;
	std::unique_ptr<VulkanBuffer> _IndexBuffer;
	std::unique_ptr<VulkanBuffer> _UniformBuffer;
	std::unique_ptr<VulkanDescriptorPool> _DescriptorPool;
	std::unique_ptr<VulkanDescriptorSetLayout> _DescriptorSetLayout;

	std::shared_ptr<VulkanPipeline> _Pipeline;

	VkDescriptorSet _UniformDescriptorSet;
	UniformBufferObject _UniformBufferObject;

	float _DeltaTime;
};