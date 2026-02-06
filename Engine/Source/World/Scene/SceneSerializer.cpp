#include "hepch.h"
#include "World/Scene/Scene.h"
#include "Utilities/UUID.h"
#include <fstream>
#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

namespace YAML {
	template<>
	struct convert<glm::vec2> {
		static Node encode(const glm::vec2& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3> {
		static Node encode(const glm::vec3& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4> {
		static Node encode(const glm::vec4& rhs) {
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs) {
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<UUID> {
		static Node encode(const UUID& uuid) {
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}
		static bool decode(const Node& node, UUID& uuid) {
			uuid = node[0].as<uint64_t>();
			return true;
		}
	};
}

namespace House {
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec) {
		out << YAML::Flow;
		out << YAML::BeginSeq << vec.x << vec.y << vec.z << vec.w << YAML::EndSeq;
		return out;
	}

	SceneSerializer::SceneSerializer(const MEM::Ref<Scene>& scene)
		: _Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap; // Entity
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();
		// Identity Component
		if (entity.HasComponent<IdentityComponent>()) {
			auto& identity = entity.GetComponent<IdentityComponent>();
			out << YAML::Key << "IdentityComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << identity.Name;
			out << YAML::EndMap;
		}

		// Transform Component
		if (entity.HasComponent<TransformComponent>()) {
			auto& transform = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Position" << YAML::Value << transform.Position;
			out << YAML::Key << "Rotation" << YAML::Value << transform.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << transform.Scale;
			out << YAML::EndMap;
		}

		// Camera Component
		if (entity.HasComponent<CameraComponent>()) {
			auto& camera = entity.GetComponent<CameraComponent>();
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "IsActive" << YAML::Value << camera.IsActive;
			out << YAML::EndMap;
		}

		// Static mesh component
		if (entity.HasComponent<StaticMeshComponent>()) {
			auto& mesh = entity.GetComponent<StaticMeshComponent>();
			out << YAML::Key << "StaticMeshComponent";
			//out << YAML::BeginMap;
			//out << YAML::Key << "ModelPath" << YAML::Value << mesh.Handle->GetFilePath().string();
			//out << YAML::EndMap;
		}

		// Directional Light Component
		if (entity.HasComponent<DirectionalLightComponent>()) {
			auto& light = entity.GetComponent<DirectionalLightComponent>();
			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << light.Handle.Color;
			out << YAML::Key << "Intensity" << YAML::Value << light.Handle.Intensity;
			out << YAML::Key << "Direction" << YAML::Value << light.Handle.Direction;
			out << YAML::EndMap;
		}

		// Point Light Component
		if (entity.HasComponent<PointLightComponent>()) {
			auto& light = entity.GetComponent<PointLightComponent>();
			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << light.Handle.Color;
			out << YAML::Key << "Intensity" << YAML::Value << light.Handle.Intensity;
			out << YAML::Key << "Position" << YAML::Value << light.Handle.Position;
			out << YAML::Key << "Radius" << YAML::Value << light.Handle.Radius;
			out << YAML::EndMap;
		}

		out << YAML::EndMap; // Entity
	}

	bool SceneSerializer::Serialize(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << _Scene->GetName();
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		for (auto entityID : _Scene->GetRegistry().view<TransformComponent, IdentityComponent>()) {
			Entity entity = { entityID, _Scene.Get() };
			if (!entity)
				continue;

			SerializeEntity(out, entity);
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
		return true;
	}

	bool SceneSerializer::Deserialize(const std::filesystem::path& path)
	{
		YAML::Node data;
		try {
			data = YAML::LoadFile(path.string());
		} catch (YAML::ParserException& e) {
			LOG_CORE_CRITICAL("Failed to load project file: {0}", e.what());
			return false;
		}

		std::string sceneName = data["Scene"].as<std::string>();
		LOG_CORE_INFO("Deserializing scene: {0}", sceneName);

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto identityComponent = entity["IdentityComponent"];
				name = identityComponent["Name"].as<std::string>();

				LOG_CORE_INFO("Deserialized entity with ID: {0}, Name: {1}", uuid, name);

				Entity& deserializedEntity = _Scene->CreateEntityWithUUID(name, uuid);

				// Transform component
				auto transformComponent = entity["TransformComponent"];
				if (transformComponent) {
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Position = transformComponent["Position"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				// Camera component
				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent) {
					bool isActive = cameraComponent["IsActive"].as<bool>();
					auto& cc = deserializedEntity.AddOrReplaceComponent<CameraComponent>(isActive);
				}

				// Static mesh component
				auto staticMeshComponent = entity["StaticMeshComponent"];
				if (staticMeshComponent) {
					std::string modelPath = staticMeshComponent["ModelPath"].as<std::string>();
					auto& smc = deserializedEntity.AddOrReplaceComponent<StaticMeshComponent>(modelPath);
				}

				// Directional light component
				auto directionalLightComponent = entity["DirectionalLightComponent"];
				if (directionalLightComponent) {
					auto& dlc = deserializedEntity.AddOrReplaceComponent<DirectionalLightComponent>();
					dlc.Handle.Color = directionalLightComponent["Color"].as<glm::vec3>();
					dlc.Handle.Intensity = directionalLightComponent["Intensity"].as<float>();
					dlc.Handle.Direction = directionalLightComponent["Direction"].as<glm::vec3>();
				}

				// Point light component
				auto pointLightComponent = entity["PointLightComponent"];
				if (pointLightComponent) {
					auto& plc = deserializedEntity.AddOrReplaceComponent<PointLightComponent>();
					plc.Handle.Color = pointLightComponent["Color"].as<glm::vec3>();
					plc.Handle.Intensity = pointLightComponent["Intensity"].as<float>();
					plc.Handle.Position = pointLightComponent["Position"].as<glm::vec3>();
					plc.Handle.Radius = pointLightComponent["Radius"].as<float>();
				}
			}
		}
		return true;
	}
}