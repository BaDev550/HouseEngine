#pragma once
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/types.h>

#include "Renderer/Model.h"
#include <filesystem>

namespace House {
	static constexpr uint32_t S_ASSIMPIMPORTERFLAGS =
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes;

	class MeshSourceImporter {
	public:
		MeshSourceImporter(const std::filesystem::path& path);
		MEM::Ref<MeshSource> ImportToMeshSource();
	private:
		const std::filesystem::path _Path;
		//Submesh ProcessSubmesh(MEM::Ref<MeshSource>& source, aiMesh* mesh, const aiScene* scene);
	};
}