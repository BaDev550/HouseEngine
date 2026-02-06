#include "hepch.h"
#include "AssetSerializer.h"
#include <yaml-cpp/yaml.h>
#include "MeshSourceImporter.h"
#include "AssetImporter.h"
#include "AssetManager.h"

#include "Renderer/Texture.h"
#include <map>

namespace YAML {
	template<>
	struct convert<std::vector<uint32_t>>
	{
		static Node encode(const std::vector<uint32_t>& value)
		{
			Node node;
			for (uint32_t element : value)
				node.push_back(element);
			return node;
		}

		static bool decode(const Node& node, std::vector<uint32_t>& result)
		{
			if (!node.IsSequence())
				return false;

			result.resize(node.size());
			for (size_t i = 0; i < node.size(); i++)
				result[i] = node[i].as<uint32_t>();

			return true;
		}
	};
}

namespace House {
	YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<uint32_t>& value)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (uint32_t element : value)
			out << element;
		out << YAML::EndSeq;
		return out;
	}

	void MeshSourceSerializer::Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {

	}
	bool MeshSourceSerializer::TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {
		MeshSourceImporter importer(metadata.FilePath);
		MEM::Ref<MeshSource> meshSource = importer.ImportToMeshSource();
		if (!meshSource)
			return false;

		asset = meshSource;
		asset->Handle = metadata.Handle;
		return true;
	}

	void MeshSerializer::Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {
		MEM::Ref<StaticMesh> mesh = asset.As<StaticMesh>();

		std::string yamlString = SerializeToFile(mesh);
		std::filesystem::path serializePath = metadata.FilePath;
		std::ofstream fout(serializePath);

		if (!fout.is_open())
			return;

		fout << yamlString;
		fout.flush();
		fout.close();
	}

	bool MeshSerializer::TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {
		std::ifstream stream(metadata.FilePath);
		std::stringstream strStream;
		strStream << stream.rdbuf();
		MEM::Ref<StaticMesh> mesh;

		bool serialized = LoadFromFile(strStream.str(), mesh);
		if (!serialized)
			return false;

		mesh->Handle = metadata.Handle;
		asset = mesh;
		return true;
	}

	std::string MeshSerializer::SerializeToFile(MEM::Ref<StaticMesh> mesh) const
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Mesh";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "MeshSource";
			out << YAML::Value << mesh->GetMeshSource();
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		return std::string(out.c_str());
	}

	bool MeshSerializer::LoadFromFile(const std::string& buff, MEM::Ref<StaticMesh>& tMesh) const
	{
		YAML::Node data = YAML::Load(buff);
		if (!data["Mesh"])
			return false;

		YAML::Node rootNode = data["Mesh"];
		if (!rootNode["MeshSource"] && !rootNode["MeshAsset"])
			return false;

		AssetHandle meshSource = rootNode["MeshSource"].as<uint64_t>();

		tMesh = MEM::Ref<StaticMesh>::Create(meshSource);
		return true;
	}

	void TextureSerializer::Serialize(const AssetMetadata& metadata, const MEM::Ref<Asset>& asset) {

	}
	bool TextureSerializer::TryLoadData(const AssetMetadata& metadata, MEM::Ref<Asset>& asset) {
		asset = Texture2D::Create(TextureSpecification(), metadata.FilePath.string());
		asset->Handle = metadata.Handle;
		return true;
	}
}