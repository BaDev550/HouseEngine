#include "hepch.h"
#include "AssetManager.h"
#include "Renderer/Texture.h"
#include "MeshSourceImporter.h"

namespace House {
	namespace Utils {
		glm::vec3 Vec3FromAIVec3(const aiVector3D& vector) {
			glm::vec3 result;
			result.x = vector.x;
			result.y = vector.y;
			result.z = vector.z;
			return result;
		}
		glm::vec3 Vec3FromAIColor(const aiColor3D& color) {
			glm::vec3 result;
			result.x = color.r;
			result.y = color.g;
			result.z = color.b;
			return result;
		}
		glm::vec2 Vec2FromAIVec2(const aiVector2D& vector) {
			glm::vec2 result;
			result.x = vector.x;
			result.y = vector.y;
			return result;
		}
	}
	MeshSourceImporter::MeshSourceImporter(const std::filesystem::path& path)
		: _Path(path)
	{
	}

	MEM::Ref<MeshSource> MeshSourceImporter::ImportToMeshSource()
	{
		MEM::Ref<MeshSource> meshSource = MEM::Ref<MeshSource>::Create();
		LOG_CORE_INFO("Loading mesh source: ", _Path.string());

		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
		const aiScene* scene = importer.ReadFile(_Path.string(), S_ASSIMPIMPORTERFLAGS);
		if (!scene) {
			LOG_CORE_WARN("	-Failed to load mesh source");
			return nullptr;
		}

		// Loading the actual mesh
		if (scene->HasMeshes()) {
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;

			meshSource->_Submeshes.reserve(scene->mNumMeshes);
			for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
				aiMesh* mesh = scene->mMeshes[m];
				bool skipLoad = !mesh->HasPositions() || !mesh->HasNormals();

				Submesh& submesh = meshSource->_Submeshes.emplace_back();
				submesh.BaseVertex = vertexCount;
				submesh.BaseIndex = indexCount;
				submesh.MaterialId = mesh->mMaterialIndex;
				submesh.VertexCount = skipLoad ? 0 : mesh->mNumVertices;
				submesh.IndexCount = skipLoad ? 0 : (mesh->mNumFaces * 3);
				submesh.Name = mesh->mName.C_Str();

				if (skipLoad) continue;

				vertexCount += mesh->mNumVertices;
				indexCount += submesh.IndexCount;
				for (size_t i = 0; i < mesh->mNumVertices; i++) {
					Vertex v;
					v.Position = Utils::Vec3FromAIVec3(mesh->mVertices[i]);
					v.Normal = Utils::Vec3FromAIVec3(mesh->mNormals[i]);

					auto& aabb = submesh.BoundingBox;
					aabb.Min.x = glm::min(v.Position.x, aabb.Min.x);
					aabb.Min.y = glm::min(v.Position.y, aabb.Min.y);
					aabb.Min.z = glm::min(v.Position.z, aabb.Min.z);
					aabb.Max.x = glm::min(v.Position.x, aabb.Min.x);
					aabb.Min.y = glm::min(v.Position.y, aabb.Min.y);
					aabb.Min.z = glm::min(v.Position.z, aabb.Min.z);

					if (mesh->HasTangentsAndBitangents()) {
						v.Tangent = Utils::Vec3FromAIVec3(mesh->mTangents[i]);
						v.Bitangent = Utils::Vec3FromAIVec3(mesh->mBitangents[i]);
					}
					if (mesh->HasTextureCoords(0)) {
						v.TexCoords = Utils::Vec3FromAIVec3(mesh->mTextureCoords[0][i]);
					}
					meshSource->_Vertices.push_back(v);
				}
				for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
					aiFace face = mesh->mFaces[i];
					for (uint32_t j = 0; j < face.mNumIndices; j++) {
						meshSource->_Indices.push_back(face.mIndices[j]);
					}
				}
			}

			if (scene->HasMaterials()) {
				MEM::Ref<Texture2D>& whiteTexture = Renderer::GetWhiteTexture();

				meshSource->_Materials.resize(scene->mNumMaterials);
				for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
					auto aiMaterial = scene->mMaterials[i];
					auto aiMaterialName = aiMaterial->GetName();
					auto modelDirectory = _Path.parent_path();
					MEM::Ref<Material> material = Material::Create(Renderer::GetShaderLibrary()->GetShader("PBRStatic"));
					MEM::Ref<MaterialAsset> ma = MEM::Ref<MaterialAsset>::Create(material);

					aiString aiTexturePath;
					glm::vec3 albedoColor = glm::vec3(1.0f);
					aiColor3D aiColor;
					if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS)
						albedoColor = Utils::Vec3FromAIColor(aiColor);
					ma->SetAlbedoColor(albedoColor); // TEMP make the material side of color vec3

					float roughness, metalness;
					if (aiMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) != AI_SUCCESS)
						roughness = 0.4f;
					if (aiMaterial->Get(AI_MATKEY_METALLIC_FACTOR, metalness) != AI_SUCCESS)
						metalness = 0.0f;
					ma->SetRoughness(roughness);
					ma->SetMetalness(metalness);

					bool hasAlbedoMap = aiMaterial->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &aiTexturePath) == AI_SUCCESS;
					if (!hasAlbedoMap)
						hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexturePath) == AI_SUCCESS;

					// Albedo map
					if (hasAlbedoMap) {
						AssetHandle textureHandle = 0;
						TextureSpecification specs{};
						specs.Attachment = false;
						specs.GenerateMipMap = true;
						specs.Filter = TextureFilter::Linear;
						specs.Wrap = TextureWrap::Clamp;
						specs.Format = TextureImageFormat::RGBA;
						auto texturePath = modelDirectory / aiTexturePath.C_Str();
						textureHandle = AssetManager::AddMemoryOnlyAsset(Texture2D::Create(specs, texturePath.string()));
						ma->SetAlbedoTexture(textureHandle);
						ma->SetAlbedoColor(glm::vec3(1.0f));
					}

					// Normal map
					bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexturePath) == AI_SUCCESS;
					if (hasNormalMap) {
						AssetHandle textureHandle = 0;
						TextureSpecification specs{};
						specs.Attachment = false;
						specs.GenerateMipMap = true;
						specs.Filter = TextureFilter::Linear;
						specs.Wrap = TextureWrap::Clamp;
						specs.Format = TextureImageFormat::RGBA;
						auto texturePath = modelDirectory / aiTexturePath.C_Str();
						textureHandle = AssetManager::AddMemoryOnlyAsset(Texture2D::Create(specs, texturePath.string()));
						ma->SetNormalTexture(textureHandle);
					}

					// Metallic map
					bool hasMetalnessMap = aiMaterial->GetTexture(aiTextureType_METALNESS, 0, &aiTexturePath) == AI_SUCCESS;
					if (hasMetalnessMap) {
						AssetHandle textureHandle = 0;
						TextureSpecification specs{};
						specs.Attachment = false;
						specs.GenerateMipMap = true;
						specs.Filter = TextureFilter::Linear;
						specs.Wrap = TextureWrap::Clamp;
						specs.Format = TextureImageFormat::RGBA;
						auto texturePath = modelDirectory / aiTexturePath.C_Str();
						textureHandle = AssetManager::AddMemoryOnlyAsset(Texture2D::Create(specs, texturePath.string()));
						ma->SetMetallicTexture(textureHandle);
					}
					AssetHandle materialHandle = AssetManager::AddMemoryOnlyAsset(ma);
					meshSource->_Materials[i] = materialHandle;
				}
			}
			else {
				// TODO- create a default texture in renderer and pass it here
			}

			if (meshSource->_Vertices.size() && meshSource->_Indices.size()) {
				meshSource->CreateBuffers();
			}
		}
		return meshSource;
	}

	//Submesh MeshSourceImporter::ProcessSubmesh(aiMesh* mesh, const aiScene* scene) {}
}