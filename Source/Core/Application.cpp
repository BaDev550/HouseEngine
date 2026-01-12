#include "hepch.h"
#include "Core/Application.h"
#include <stdexcept>

Application* Application::_Instance = nullptr;
Application::Application()
{
	if (_Instance)
		return;
	_Instance = this;

	WindowConfig config{};
	config.Title = APPLICATIONNAME;
	config.Width = 800;
	config.Height = 800;
	
	_Window = std::make_unique<Window>(config);
	_VulkanContext = std::make_unique<VulkanContext>();
	_Renderer = std::make_unique<Renderer>();
	_Texture = std::make_shared<VulkanTexture>("Resources/Textures/texture.jpg");

	CreateVertexBuffer();
	CreateIndexBuffer();

	_DescriptorPool = VulkanDescriptorPool::Builder()
		.SetMaxSets(1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000)
		.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000)
		.Build();
	_DescriptorSetLayouts[0] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		.Build();
	_DescriptorSetLayouts[1] = VulkanDescriptorSetLayout::Builder()
		.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
		.Build();

	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	_UniformBuffer = std::make_unique<VulkanBuffer>(bufferSize, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_UniformBuffer->Map();

	VkDescriptorBufferInfo camInfo = _UniformBuffer->DescriptorInfo();
	VkDescriptorImageInfo imageInfo = _Texture->GetImageDescriptorInfo();
	VulkanDescriptorWriter(*_DescriptorSetLayouts[0], *_DescriptorPool)
		.WriteBuffer(0, &camInfo)
		.Build(_UniformDescriptorSet);
	VulkanDescriptorWriter(*_DescriptorSetLayouts[1], *_DescriptorPool)
		.WriteImage(0, &imageInfo)
		.Build(_TextureDescriptorSet);

	VulkanPipelineConfig vulkanConfig{};
	VulkanContext::DefaultPipelineConfigInfo(vulkanConfig);
	vulkanConfig.RenderPass = _Renderer->GetSwapChainRenderPass();
	_Pipeline = std::make_shared<VulkanPipeline>(vulkanConfig, "Shaders/base.vert", "Shaders/base.frag");
}

Application::~Application()
{
	_Renderer = nullptr;
	_Window = nullptr;

	_UniformBuffer = nullptr;
	_VertexBuffer = nullptr;
	_IndexBuffer = nullptr;

	_Texture = nullptr;
	_Pipeline = nullptr;
	_DescriptorPool = nullptr;

	for (size_t i = 0; i < _DescriptorSetLayouts.size(); i++)
		_DescriptorSetLayouts[i] = nullptr;

	_VulkanContext = nullptr;
}

std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f},  {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}
};

std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0
};

void Application::Run()
{
	while (!_Window->ShouldClose()) {
		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		_DeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		_Window->PollEvents();

		_UniformBufferObject.Model = glm::rotate(glm::mat4(1.0f), _DeltaTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_UniformBufferObject.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		_UniformBufferObject.Proj = glm::perspective(glm::radians(45.0f), (float)_Renderer->GetSwapChainExtent().width / (float)_Renderer->GetSwapChainExtent().height, 0.01f, 10.0f);
		_UniformBuffer->WriteToBuffer(&_UniformBufferObject);

		_Renderer->Submit([&](VkCommandBuffer& cmd) {
			_Pipeline->Bind(cmd);

			VkBuffer vertexBuffers[] = { _VertexBuffer->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_Pipeline->GetPipelineLayout(),
				0, 1,
				&_UniformDescriptorSet,
				0, nullptr);
			vkCmdBindDescriptorSets(
				cmd,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_Pipeline->GetPipelineLayout(),
				1, 1,
				&_TextureDescriptorSet,
				0, nullptr);
			vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
			vkCmdBindIndexBuffer(cmd, _IndexBuffer->GetBuffer(), offsets[0], VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
			});
	}
	_VulkanContext->WaitToDeviceIdle();
}

void Application::CreateVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();
	std::unique_ptr<VulkanBuffer> stagingBuffer = std::make_unique<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer->Map();
	stagingBuffer->WriteToBuffer(vertices.data());
	stagingBuffer->Unmap();

	_VertexBuffer = std::make_unique<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_VulkanContext->CopyBuffer(stagingBuffer->GetBuffer(), _VertexBuffer->GetBuffer(), bufferSize);
}

void Application::CreateIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();
	std::unique_ptr<VulkanBuffer> stagingBuffer = std::make_unique<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer->Map();
	stagingBuffer->WriteToBuffer(indices.data());
	stagingBuffer->Unmap();

	_IndexBuffer = std::make_unique<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_VulkanContext->CopyBuffer(stagingBuffer->GetBuffer(), _IndexBuffer->GetBuffer(), bufferSize);
}
