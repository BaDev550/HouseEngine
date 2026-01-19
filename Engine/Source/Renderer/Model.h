#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/glm.hpp>
#include <array>
#include <filesystem>
#include <Vulkan/vulkan.h>

#include "Buffer.h"
#include "Material.h"

namespace House {
	struct MeshPushConstants { // Only used for Vulkan
		glm::mat4 Transform;
	};

	struct AABB {
		glm::vec3 Min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		glm::vec3 Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		void Merge(const glm::vec3& point) {
			Min = glm::min(Min, point);
			Max = glm::max(Max, point);
		}

		glm::vec3 GetCenter() const { return (Min + Max) * 0.5f; }
		glm::vec3 GetExtents() const { return (Max - Min) * 0.5f; }
	};

	template<typename T, typename... Rest>
	void HashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e37779b9 + (seed << 6) + (seed >> 2);
		(HashCombine(seed, rest), ...);
	}

	struct Vertex {
		glm::vec3 Position;
		glm::vec2 TexCoords;
		glm::vec3 Normal;

		bool operator=(const Vertex& other) const {
			return Position == other.Position &&
				TexCoords == other.TexCoords &&
				Normal == other.Normal;
		}

		Vertex() = default;
	};

	static constexpr uint32_t S_ASSIMPIMPORTERFLAGS =
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices | 
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes;

	class Mesh {
	public:
		Mesh(
			const std::string& name,
			std::vector<Vertex>& vertices,
			std::vector<uint32_t>& indices,
			const uint32_t materialID);
		~Mesh() = default;
		const std::string& GetName() const { return _Name; }
		const uint32_t GetMaterialID() const { return _MaterialId; }
		const uint32_t GetIndexCount() const { return _IndexCount; }
		const MEM::Ref<Buffer>& GetVertexBuffer() const { return _VertexBuffer; }
		const MEM::Ref<Buffer>& GetIndexBuffer() const { return _IndexBuffer; }
		const AABB& GetBoundingBox() const { return _BoundingBox; }
	private:
		std::string _Name = "EMPTY_MESH";
		MEM::Ref<Buffer> _VertexBuffer;
		MEM::Ref<Buffer> _IndexBuffer;
		std::vector<Vertex> _Vertices;
		std::vector<uint32_t> _Indices;
		uint32_t _MaterialId = UINT32_MAX;
		uint32_t _VertexCount = 0;
		uint32_t _IndexCount = 0;
		AABB _BoundingBox;

		bool _IsVisible = true;

		void CreateVertexBuffer(std::vector<Vertex>& vertices);
		void CreateIndexBuffer(std::vector<uint32_t>& indices);

		friend class Model;
	};

	class Model : public MEM::RefCounted {
	public:
		Model(const std::filesystem::path& path) { LoadModelFromFile(path); }
		~Model();

		MEM::Ref<Material>& GetMaterialByID(uint32_t id) { return _Materials[id]; }
		std::vector<Mesh>& GetMeshes() { return _Meshes; }
		std::unordered_map<uint32_t, MEM::Ref<Material>>& GetMaterials() { return _Materials; }
		const AABB& GetBoundingBox() const { return _BoundingBox; }
	private:
		void LoadModelFromFile(const std::filesystem::path& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		void ProcessMaterials(const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		std::filesystem::path _ModelDirectory = "EMPTY_MODEL_DIRECTORY";
		std::vector<Mesh> _Meshes;
		AABB _BoundingBox;
		std::unordered_map<uint32_t, MEM::Ref<Material>> _Materials;
	};
}