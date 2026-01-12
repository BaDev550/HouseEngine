#include "hepch.h"
#include "Model.h"
#include "Core/Application.h"

Mesh::Mesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const uint32_t materialID)
	: _Name(name), _MaterialId(materialID), _Context(*Application::Get()->GetVulkanContext())
{
	CreateVertexBuffer(vertices);
	CreateIndexBuffer(indices);
}

void Mesh::CreateVertexBuffer(std::vector<Vertex>& vertices)
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
	_Context.CopyBuffer(stagingBuffer->GetBuffer(), _VertexBuffer->GetBuffer(), bufferSize);
}

void Mesh::CreateIndexBuffer(std::vector<uint32_t>& indices)
{
	VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();
	MEM::Scope<VulkanBuffer> stagingBuffer = MEM::MakeScope<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer->Map();
	stagingBuffer->WriteToBuffer(indices.data());
	stagingBuffer->Unmap();

	_IndexBuffer = MEM::MakeScope<VulkanBuffer>(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_Context.CopyBuffer(stagingBuffer->GetBuffer(), _IndexBuffer->GetBuffer(), bufferSize);
}

void Model::LoadModelFromFile(const std::string& path) {

}

void Model::ProcessMaterials(const aiScene* scene) {

}

void Model::ProcessNode(aiNode* node) {

}