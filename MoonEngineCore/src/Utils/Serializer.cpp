#include "mpch.h"

#include "Core/ResourceManager.h"

#include "Engine/Components.h"
#include "Engine/Scene.h"

#include "Renderer/Texture.h"

#include "Utils/Serializer.h"

#include <fstream>

#include <yaml-cpp/yaml.h>
#include <yaml-cpp/dll.h>

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

	bool Serializer::IsValid(const std::string& filepath, const char* fileFormat)
	{
		if (filepath.substr(filepath.find_last_of(".") + 1) == fileFormat)
		{
			YAML::Node data;
			data = YAML::LoadFile(filepath);
			if (!data["Scene"])
				return false;
			return true;
		}
		return false;
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
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
			out << YAML::Key << "Size" << YAML::Value << component.Size;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteComponent>())
		{
			out << YAML::Key << "SpriteComponent";
			out << YAML::BeginMap;

			const SpriteComponent& spriteComponent = entity.GetComponent<SpriteComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteComponent.Color;
			
			if (spriteComponent.Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << spriteComponent.Texture->Filepath;
			else
				out << YAML::Key << "TexturePath" << YAML::Value << "null";

			out << YAML::EndMap;
		}

		if (entity.HasComponent<ParticleComponent>())
		{
			out << YAML::Key << "ParticleComponent";
			out << YAML::BeginMap;

			const ParticleComponent& particleComponent = entity.GetComponent<ParticleComponent>();

			//Lifecycle
			out << YAML::Key << "Play" << YAML::Value << particleComponent.Play;
			out << YAML::Key << "AutoPlay" << YAML::Value << particleComponent.AutoPlay;
			out << YAML::Key << "Duration" << YAML::Value << particleComponent.Duration;

			out << YAML::Key << "MaxParticles" << YAML::Value << particleComponent.MaxParticles;
			out << YAML::Key << "BurstMode" << YAML::Value << particleComponent.BurstMode;
			out << YAML::Key << "Rate" << YAML::Value << particleComponent.Rate;
			

			out << YAML::Key << "Lifetime" << YAML::Value << particleComponent.Lifetime;

			//Position
			out << YAML::Key << "Direction" << YAML::Value << particleComponent.Direction;
			out << YAML::Key << "DirectionVelocity" << YAML::Value << particleComponent.DirectionVelocity;
			out << YAML::Key << "ShapeSize" << YAML::Value << particleComponent.ShapeSize;

			//Size
			out << YAML::Key << "SizeStart" << YAML::Value << particleComponent.SizeStart;
			out << YAML::Key << "SizeEnd" << YAML::Value << particleComponent.SizeEnd;
			out << YAML::Key << "SizeChangeSpeed" << YAML::Value << particleComponent.SizeChangeSpeed;
			
			//Color
			if (particleComponent.Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << particleComponent.Texture->Filepath;
			else
				out << YAML::Key << "TexturePath" << YAML::Value << "null";

			out << YAML::Key << "ColorStart" << YAML::Value << particleComponent.ColorStart;
			out << YAML::Key << "ColorEnd" << YAML::Value << particleComponent.ColorEnd;
			out << YAML::Key << "ColorChangeSpeed" << YAML::Value << particleComponent.ColorChangeSpeed;

			//Physics
			out << YAML::Key << "UsePhysics" << YAML::Value << particleComponent.UsePhysics;
			out << YAML::Key << "Gravity" << YAML::Value << particleComponent.Gravity;
			out << YAML::Key << "Mass" << YAML::Value << particleComponent.Mass;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			const CameraComponent& cameraComponent = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "IsMain" << YAML::Value << cameraComponent.IsMain;
			out << YAML::Key << "Distance" << YAML::Value << cameraComponent.Distance;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	void Serializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << m_Scene->SceneName;
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		std::vector<entt::entity> entites;
		m_Scene->m_Registry.each([&](auto entityID)
		{
			entites.push_back(entityID);
		});

		for (auto e = entites.end(); e != entites.begin();)
		{
			Entity entity{ (*--e), m_Scene.get() };
			if (!entity)
				return;
			SerializeEntity(out, entity);
		}
		
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
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
		m_Scene->SceneName = sceneName;

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
						component.Texture = ResourceManager::LoadTexture(texturePath);
				}
				else
					deserializedEntity.RemoveComponent<SpriteComponent>();

				auto particleComponent = entity["ParticleComponent"];
				if (particleComponent)
				{
					ParticleComponent& component = deserializedEntity.AddComponent<ParticleComponent>();
					
					GetIfExists<bool>(&component.Play, particleComponent, "Play");
					GetIfExists<bool>(&component.AutoPlay,particleComponent, "AutoPlay");
					GetIfExists<float>(&component.Duration, particleComponent, "Duration");

					GetIfExists<int>(&component.MaxParticles, particleComponent, "MaxParticles");
					GetIfExists<bool>(&component.BurstMode, particleComponent, "BurstMode");
					GetIfExists<int>(&component.Rate, particleComponent, "Rate");
					
					GetIfExists<float>(&component.Lifetime, particleComponent, "Lifetime");
					GetIfExists<glm::vec3>(&component.Direction, particleComponent, "Direction");
					GetIfExists<glm::vec3>(&component.DirectionVelocity, particleComponent, "DirectionVelocity");
					GetIfExists<glm::vec3>(&component.ShapeSize, particleComponent, "ShapeSize");
					
					GetIfExists<glm::vec3>(&component.SizeStart, particleComponent, "SizeStart");
					GetIfExists<glm::vec3>(&component.SizeEnd, particleComponent, "SizeEnd");
					GetIfExists<float>(&component.SizeChangeSpeed, particleComponent, "SizeChangeSpeed");

					GetIfExists<glm::vec4>(&component.ColorStart, particleComponent, "ColorStart");
					GetIfExists<glm::vec4>(&component.ColorEnd, particleComponent, "ColorEnd");
					GetIfExists<float>(&component.ColorChangeSpeed, particleComponent, "ColorChangeSpeed");
					
					GetIfExists<bool>(&component.UsePhysics, particleComponent, "UsePhysics");
					GetIfExists<float>(&component.Gravity, particleComponent, "Gravity");
					GetIfExists<float>(&component.Mass, particleComponent, "Mass");
				
					std::string texturePath;
					if(GetIfExists<std::string>(&texturePath, particleComponent, "TexturePath"))
						if (texturePath != "null")
							component.Texture = ResourceManager::LoadTexture(texturePath);

					component.Resize();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					CameraComponent& component = deserializedEntity.AddComponent<CameraComponent>();
					component.IsMain = cameraComponent["IsMain"].as<bool>();
					component.Distance = cameraComponent["Distance"].as<float>();
				}
			}
		}
		return true;
	}
}