#include "hepch.h"
#include "Model.h"
#include "Core/Application.h"

namespace House {
	Mesh::Mesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const uint32_t materialID)
		: _Name(name), _MaterialId(materialID)
	{
		_IndexCount = indices.size();
		CreateVertexBuffer(vertices);
		CreateIndexBuffer(indices);
	}

	void Mesh::CreateVertexBuffer(std::vector<Vertex>& vertices)
	{
		uint64_t bufferSize = sizeof(Vertex) * vertices.size();
		MEM::Ref<Buffer> stagingBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferSrc,
			MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT
		);
		stagingBuffer->Map();
		stagingBuffer->WriteToBuffer(vertices.data());
		stagingBuffer->Unmap();

		_VertexBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferDst | BufferType::VertexBuffer,
			MemoryProperties::DEVICE
		);
		Renderer::CopyBuffer(stagingBuffer, _VertexBuffer, bufferSize);
	}

	void Mesh::CreateIndexBuffer(std::vector<uint32_t>& indices)
	{
		uint64_t bufferSize = sizeof(uint32_t) * indices.size();
		MEM::Ref<Buffer> stagingBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferSrc,
			MemoryProperties::HOST_VISIBLE | MemoryProperties::HOST_COHERENT
		);
		stagingBuffer->Map();
		stagingBuffer->WriteToBuffer(indices.data());
		stagingBuffer->Unmap();

		_IndexBuffer = Buffer::Create(
			bufferSize,
			BufferType::TransferDst | BufferType::IndexBuffer,
			MemoryProperties::DEVICE
		);
		Renderer::CopyBuffer(stagingBuffer, _IndexBuffer, bufferSize);
	}

	Model::~Model() {
		_Meshes.clear();
		_Materials.clear();
		LOG_RENDERER_INFO("Model destroyed");
	}

	void Model::LoadModelFromFile(const std::filesystem::path& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.string(), S_ASSIMPIMPORTERFLAGS);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			LOG_RENDERER_ERROR("Failed to laod model from : {}", path.string());
			return;
		}
		_ModelDirectory = path.parent_path();

		ProcessMaterials(scene);
		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessMaterials(const aiScene* scene) {
		for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* aiMat = scene->mMaterials[i];

			auto material = Material::Create(Renderer::GetPipeline("MainShader"));
			aiString path;

			if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
				std::filesystem::path texturePath = _ModelDirectory / path.C_Str();
				MEM::Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
				material->GetMaterialData().DiffuseTexture = texture;
			}
			else {
				material->GetMaterialData().DiffuseTexture = Renderer::GetWhiteTexture();
			}
			if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
				std::filesystem::path texturePath = _ModelDirectory / path.C_Str();
				MEM::Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
				material->GetMaterialData().NormalTexture = texture;
			}
			else {
				material->GetMaterialData().NormalTexture = Renderer::GetWhiteTexture();
			}
			material->Build();

			material->_Id = i;
			_Materials[i] = (material);
		}
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene) {
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			_Meshes.push_back(ProcessMesh(mesh, scene));
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex v;
			v.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			v.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			if (mesh->mTextureCoords[0]) {
				v.TexCoords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			vertices.push_back(v);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		return Mesh(mesh->mName.C_Str(), vertices, indices, mesh->mMaterialIndex);
	}
}