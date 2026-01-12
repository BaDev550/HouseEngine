#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/glm.hpp>
#include <array>
#include <Vulkan/vulkan.h>

#include "Vulkan/VulkanBuffer.h"
#include "Material.h"

template<typename T, typename... Rest>
void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v)+0x9e37779b9 + (seed << 6) + (seed >> 2);
	(HashCombine(seed, rest), ...);
}

struct Vertex {
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;

	static VkVertexInputBindingDescription GetBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, TexCoords);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Normal);
		return attributeDescriptions;
	}

	bool operator=(const Vertex& other) const {
		return Position == other.Position &&
			TexCoords == other.TexCoords &&
			Normal == other.Normal;
	}

	Vertex() = default;
};

static constexpr uint32_t S_ASSIMPIMPORTERFLAGS =
	aiProcess_Triangulate |
	aiProcess_GenSmoothNormals |
	aiProcess_GenUVCoords |
	aiProcess_FlipUVs |
	aiProcess_CalcTangentSpace;

class Mesh {
public:
	Mesh(
		const std::string& name,
		std::vector<Vertex>& vertices,
		std::vector<uint32_t>& indices,
		const uint32_t materialID);
	~Mesh() = default;
	void Bind(VkCommandBuffer cmd);
	const std::string& GetName() const { return _Name; }
	const uint32_t GetMaterialID() const { return _MaterialId; }
private:
	std::string _Name = "EMPTY_MESH";
	MEM::Ref<VulkanBuffer> _VertexBuffer;
	MEM::Ref<VulkanBuffer> _IndexBuffer;
	std::vector<Vertex> _Vertices;
	std::vector<uint32_t> _Indices;
	uint32_t _MaterialId = UINT32_MAX;
	uint32_t _VertexCount = 0;
	uint32_t _IndexCount = 0;
	VulkanContext& _Context;

	bool _IsVisible = true;

	void CreateVertexBuffer(std::vector<Vertex>& vertices);
	void CreateIndexBuffer(std::vector<uint32_t>& indices);
};

class Model {
public:
	Model(const std::string& path) { LoadModelFromFile(path); }

	std::vector<Mesh>& GetMeshes() { return _Meshes; }
	std::unordered_map<uint32_t, Material>& GetMaterials() { return _Materials; }
private:
	void LoadModelFromFile(const std::string& path);
	void ProcessNode(aiNode* node);
	void ProcessMaterials(const aiScene* scene);

	std::string _ModelDirectory = "EMPTY_MODEL_DIRECTORY";
	std::vector<Mesh> _Meshes;
	std::unordered_map<uint32_t, Material> _Materials;
};