#include "hepch.h"
#include "Model.h"
#include "Core/Application.h"

Mesh::Mesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const uint32_t materialID)
	: _Name(name), _MaterialId(materialID), _Context(Application::Get()->GetVulkanContext())
{
	_IndexCount = indices.size();
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

	_VertexBuffer = MEM::Ref<VulkanBuffer>::Create(
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

	_IndexBuffer = MEM::Ref<VulkanBuffer>::Create(
		bufferSize,
		1,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	_Context.CopyBuffer(stagingBuffer->GetBuffer(), _IndexBuffer->GetBuffer(), bufferSize);
}

void Model::LoadModelFromFile(const std::filesystem::path& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), S_ASSIMPIMPORTERFLAGS);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_CORE_ERROR("Failed to laod model from : {}", path.string());
		return;
	}
	_ModelDirectory = path.parent_path();

	ProcessMaterials(scene);
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessMaterials(const aiScene* scene) {
	for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* aiMat = scene->mMaterials[i];
		
		auto& pipeline = Renderer::GetPipelineLibrary()->GetPipeline("MainPipeline");
		auto material = MEM::Ref<Material>::Create(pipeline);
		aiString path;
		
		if (aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
			std::filesystem::path texturePath = _ModelDirectory / path.C_Str();
			MEM::Ref<VulkanTexture> texture = MEM::Ref<VulkanTexture>::Create(texturePath.string());
			material->GetMaterialVariables().DiffuseTexture = texture;
		}
		else {
			material->GetMaterialVariables().DiffuseTexture = Renderer::GetWhiteTexture();
		}
		if (aiMat->GetTexture(aiTextureType_NORMALS, 0, &path) == AI_SUCCESS) {
			std::filesystem::path texturePath = _ModelDirectory / path.C_Str();
			MEM::Ref<VulkanTexture> texture = MEM::Ref<VulkanTexture>::Create(texturePath.string());
			material->GetMaterialVariables().NormalTexture = texture;
		}
		else {
			material->GetMaterialVariables().NormalTexture = Renderer::GetWhiteTexture();
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
