#include "mpch.h"
#include "Serializer.h"

#include "Engine/Entity.h"
#include "Engine/Components.h"

#include <fstream>

#include "yaml-cpp/yaml.h"
#include "yaml-cpp/dll.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace MoonEngine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	Serializer::Serializer(const Ref<Scene>& scene)
		: m_Scene(scene)
	{}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		auto& uuid = entity.GetComponent<UUIDComponent>();
		out << YAML::Key << "Entity" << YAML::Value << uuid.ID.str();

		if (entity.HasComponent<IdentityComponent>())
		{
			out << YAML::Key << "IdentityComponent";
			out << YAML::BeginMap;

			auto& name = entity.GetComponent<IdentityComponent>().Name;
			out << YAML::Key << "Name" << YAML::Value << name;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			TransformComponent& component = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << component.Position;
			out << YAML::Key << "Rotation" << YAML::Value << component.Rotation;
			out << YAML::Key << "Size" << YAML::Value << component.Size;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "IsMain" << YAML::Value << cameraComponent.isMain;
			out << YAML::Key << "Distance" << YAML::Value << cameraComponent.Distance;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;

			SpriteComponent& spriteComponent = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteComponent.Color;
			if(spriteComponent.Texture)
					out << YAML::Key << "TexturePath" << YAML::Value << spriteComponent.Texture->Filepath;
			else
					out << YAML::Key << "TexturePath" << YAML::Value << "null";

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void Serializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			Entity entity = { entityID };
			if (!entity)
				return;
			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool Serializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Entity deserializedEntity = m_Scene->CreateEntity();

				auto& uuidComponent = deserializedEntity.GetComponent<UUIDComponent>();
				uuidComponent.ID.fromStr(entity["Entity"].as<std::string>().c_str());
				
				std::string name;
				auto idComponent = entity["IdentityComponent"];
				if (idComponent)
					name = idComponent["Name"].as<std::string>();
				deserializedEntity.GetComponent<IdentityComponent>().Name = name;

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					TransformComponent& component = deserializedEntity.GetComponent<TransformComponent>();
					component.Position = transformComponent["Position"].as<glm::vec3>();
					component.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					component.Size = transformComponent["Size"].as<glm::vec3>();
				}

				auto spriteComponent = entity["SpriteComponent"];
				if (spriteComponent)
				{
					SpriteComponent& component = deserializedEntity.GetComponent<SpriteComponent>();
					component.Color = spriteComponent["Color"].as<glm::vec4>();
					auto& texturePath = spriteComponent["TexturePath"].as<std::string>();
					if (texturePath != "null")
						component.Texture = CreateRef<Texture>(spriteComponent["TexturePath"].as<std::string>());
				}
				else
					deserializedEntity.RemoveComponent<SpriteComponent>();

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& component = deserializedEntity.AddComponent<CameraComponent>();

					component.isMain = cameraComponent["IsMain"].as<bool>();
					component.Distance = cameraComponent["Distance"].as<float>();
				}
			}
		}
		return true;
	}
}