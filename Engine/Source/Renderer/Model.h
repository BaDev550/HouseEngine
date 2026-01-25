#pragma once
#include <glm/glm.hpp>
#include <array>
#include <filesystem>
#include "AssetManager/Asset.h"

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
		glm::vec3 Tangent;
		glm::vec3 Bitangent;

		bool operator=(const Vertex& other) const {
			return Position == other.Position &&
				TexCoords == other.TexCoords &&
				Normal == other.Normal;
		}

		Vertex() = default;
	};

	struct Submesh {
		std::string Name = "EMPTY_MESH";
		uint32_t MaterialId = UINT32_MAX;
		uint32_t BaseVertex = 0;
		uint32_t BaseIndex = 0;
		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;
		AABB BoundingBox;

		bool IsVisible = true;
	};
	
	class MeshSource : public Asset {
	public:
		MeshSource() = default;
		MeshSource(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
		const std::vector<Vertex>& GetVertices() const { return _Vertices; }
		const std::vector<uint32_t>& GetIndices() const { return _Indices; }
		const MEM::Ref<Buffer>& GetVertexBuffer() const { return _VertexBuffer; }
		const MEM::Ref<Buffer>& GetIndexBuffer() const { return _IndexBuffer; }
		const std::filesystem::path& GetModelDirectory() const { return _Directory; }
		const std::filesystem::path& GetFilePath() const { return _Path; }
		const AABB& GetBoundingBox() const { return _BoundingBox; }
		std::vector<Submesh>& GetSubmeshes() { return _Submeshes; }

		static AssetType GetStaticAssetType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
	private:
		AABB _BoundingBox;
		std::vector<Submesh> _Submeshes;
		MEM::Ref<Buffer> _VertexBuffer;
		MEM::Ref<Buffer> _IndexBuffer;
		std::vector<Vertex> _Vertices;
		std::vector<uint32_t> _Indices;
		std::vector<AssetHandle> _Materials;
		std::filesystem::path _Directory = "EMPTY_MODEL_DIRECTORY";
		std::filesystem::path _Path = "EMPTY_MODEL_PATH";

		void CreateBuffers();
		friend class MeshSourceImporter;
	};

	class StaticMesh : public Asset {
	public:
		StaticMesh(AssetHandle meshSource);
		~StaticMesh();

		std::unordered_map<uint32_t, MEM::Ref<Material>>& GetMaterials() { return _Materials; }
		MEM::Ref<Material>& GetMaterialByID(uint32_t id) { return _Materials[id]; }
	private:
		AssetHandle _MeshSource = INVALID_ASSET_HANDLE;
		std::unordered_map<uint32_t, MEM::Ref<Material>> _Materials;
	};
}