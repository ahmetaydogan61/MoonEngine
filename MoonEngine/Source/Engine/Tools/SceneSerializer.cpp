#include "mpch.h"
#include "Engine/Tools/SceneSerializer.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/dll.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

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
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

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

	void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		const auto& uuid = entity.GetComponent<UUIDComponent>();
		out << YAML::Key << "Entity" << YAML::Value << uuid.ID.str();

		if (entity.HasComponent<IdentityComponent>())
		{
			out << YAML::Key << "IdentityComponent";
			out << YAML::BeginMap;

			const auto& name = entity.GetComponent<IdentityComponent>().Name;
			out << YAML::Key << "Name" << YAML::Value << name;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			const TransformComponent& component = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Position" << YAML::Value << component.Position;
			out << YAML::Key << "Rotation" << YAML::Value << component.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << component.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;

			const SpriteComponent& spriteComponent = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteComponent.Color;
			out << YAML::Key << "Tiling" << YAML::Value << spriteComponent.Tiling;

			if (spriteComponent.Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << spriteComponent.Texture->GetPath().string();
			else
				out << YAML::Key << "TexturePath" << YAML::Value << "null";

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			const CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();
			out << YAML::Key << "IsMain" << YAML::Value << cameraComponent.IsMain;
			out << YAML::Key << "Size" << YAML::Value << cameraComponent.Size;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const Shared<Scene>& scene, const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << scene->SceneName;

		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		scene->m_Registry.each_reverse([&](auto entityID)
		{
			{
				Entity e{ entityID, scene.get() };
				if (!e)
					return;
				SerializeEntity(out, e);
			}
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(path);
		fout << out.c_str();
	}

	template<typename T>
	bool GetIfExists(T* value, const YAML::Node& node, const char* label)
	{
		auto& subnode = node[label];
		if (subnode)
		{
			*value = subnode.as<T>();
			return true;
		}
		return false;
	}

	void SceneSerializer::Deserialize(const Shared<Scene>& scene, const std::filesystem::path& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException e)
		{
			return;
		}

		if (!data["Scene"])
			return;

		std::string sceneName = data["Scene"].as<std::string>();
		const std::string& sceneFileName = path.filename().string();
		size_t lastindex = sceneFileName.find_last_of(".");
		scene->SceneName = sceneFileName.substr(0, lastindex);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Entity deserializedEntity = scene->CreateEntity();

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
					component.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto spriteComponent = entity["SpriteComponent"];
				if (spriteComponent)
				{
					SpriteComponent& component = deserializedEntity.GetComponent<SpriteComponent>();
					component.Color = spriteComponent["Color"].as<glm::vec4>();
					const auto& texturePath = spriteComponent["TexturePath"].as<std::string>();
					if (texturePath != "null")
						component.Texture = MakeShared<Texture>(texturePath);
				}
				else
					deserializedEntity.RemoveComponent<SpriteComponent>();

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					CameraComponent& component = deserializedEntity.AddComponent<CameraComponent>();
					component.IsMain = cameraComponent["IsMain"].as<bool>();
					component.Size = cameraComponent["Size"].as<float>();
				}
			}
		}
	}
}