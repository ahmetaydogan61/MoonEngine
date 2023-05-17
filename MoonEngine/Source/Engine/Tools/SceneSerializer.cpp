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

	YAML::Emitter& operator<<(YAML::Emitter& out, const Shared<MoonEngine::Texture>& t)
	{
		if (t == nullptr)
			out << "null";
		else
			out << t->GetPath().string();
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, PhysicsBodyComponent::BodyType bt)
	{
		std::string btName;
		switch (bt)
		{
			case MoonEngine::PhysicsBodyComponent::BodyType::Static:
				btName = "Static";
				break;
			case MoonEngine::PhysicsBodyComponent::BodyType::Dynamic:
				btName = "Dynamic";
				break;
			case MoonEngine::PhysicsBodyComponent::BodyType::Kinematic:
				btName = "Kinematic";
				break;
			default:
				btName = "Invalid";
				break;
		}

		out << btName;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, EmitterType et)
	{
		out << (int)et;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, SortMode sm)
	{
		out << (int)sm;
		return out;
	}

	struct YAMLSerializer
	{
		YAMLSerializer(YAML::Emitter& out)
			:output(out) {}

		YAML::Emitter& output;

		void BeginParse(const std::string& key)
		{
			output << YAML::Key << key;
			output << YAML::BeginMap;
		}

		void EndParse() { output << YAML::EndMap; }

		template<class T>
		auto Serialize(T& obj) -> decltype(obj.reflect(*this), void()) {

			obj.reflect(*this);
		}

		template<class T>
		YAMLSerializer& operator()(const char* propertyID, T& field) {
			output << YAML::Key << propertyID << YAML::Value << field;
			return *this;
		}
	};

	struct YAMLDeserializer
	{
		YAMLDeserializer(YAML::Node& node)
			:Node(node) {}

		YAML::Node& Node;

		template<class T>
		auto Deserialize(T& obj) -> decltype(obj.reflect(*this), void()) {

			obj.reflect(*this);
		}

		YAMLDeserializer& operator()(const char* propertyID, PhysicsBodyComponent::BodyType& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto type = propNode.as<std::string>();
			if (type == "Static")
				field = PhysicsBodyComponent::BodyType::Static;
			else if (type == "Dynamic")
				field = PhysicsBodyComponent::BodyType::Dynamic;
			else if (type == "Kinematic")
				field = PhysicsBodyComponent::BodyType::Kinematic;

			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, SortMode& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto type = propNode.as<int>();
			field = (SortMode)type;
			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, EmitterType& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto type = propNode.as<int>();
			field = (EmitterType)type;
			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, Shared<Texture>& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto path = propNode.as<std::string>();

			if (path != "null")
				field = MakeShared<Texture>(path);

			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, glm::vec2& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto vec = propNode.as<std::vector<float>>();
			if (vec.size() == 1)
				field = glm::vec2(vec[0]);
			else if (vec.size() == 2)
				field = glm::make_vec2(vec.data());
			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, glm::vec3& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto vec = propNode.as<std::vector<float>>();
			if (vec.size() == 1)
				field = glm::vec3(vec[0]);
			else if (vec.size() == 3)
				field = glm::make_vec3(vec.data());
			return *this;
		}

		YAMLDeserializer& operator()(const char* propertyID, glm::vec4& field) {
			auto propNode = Node[propertyID];
			if (!propNode)
				return *this;

			auto vec = propNode.as<std::vector<float>>();
			if (vec.size() == 1)
				field = glm::vec4(vec[0]);
			else if (vec.size() == 4)
				field = glm::make_vec4(vec.data());
			return *this;
		}

		template<class T>
		YAMLDeserializer& operator()(const char* propertyID, T& field) {
			auto propNode = Node[propertyID];
			if (propNode)
				field = propNode.as<T>();
			return *this;
		}
	};

	template<typename T>
	void SerializeIfExists(YAML::Emitter& out, Entity& entity)
	{
		if (entity.HasComponent<T>())
		{
			YAMLSerializer parser(out);
			T& c = entity.GetComponent<T>();
			parser.BeginParse(typeid(T).name());
			parser.Serialize(c);
			parser.EndParse();
		}
	}

	void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		const auto& uuid = entity.GetComponent<UUIDComponent>();
		out << YAML::Key << "Entity" << YAML::Value << uuid.ID.str();

		SerializeIfExists<IdentityComponent>(out, entity);
		SerializeIfExists<TransformComponent>(out, entity);
		SerializeIfExists<CameraComponent>(out, entity);
		SerializeIfExists<SpriteComponent>(out, entity);
		SerializeIfExists<ScriptComponent>(out, entity);
		SerializeIfExists<PhysicsBodyComponent>(out, entity);

		if (entity.HasComponent<ParticleComponent>())
		{
			YAMLSerializer parser(out);
			ParticleComponent& c = entity.GetComponent<ParticleComponent>();
			parser.BeginParse(typeid(ParticleComponent).name());
			parser.Serialize(c.ParticleSystem);
			parser.Serialize(c.Particle);
			parser.EndParse();
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
	T* GetIfExists(YAML::Node& node, Entity& entity)
	{
		auto componentNode = node[typeid(T).name()];
		if (componentNode)
		{
			YAMLDeserializer deserializer(componentNode);
			if (entity.HasComponent<T>())
			{
				T& component = entity.GetComponent<T>();
				deserializer.Deserialize(component);
				return &component;
			}
			else
			{
				T& component = entity.AddComponent<T>();
				deserializer.Deserialize(component);
				return &component;
			}
		}
		return nullptr;
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
		scene->SceneName = sceneName;

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				Entity deserializedEntity = scene->CreateEntity();

				auto& uuidComponent = deserializedEntity.GetComponent<UUIDComponent>();
				uuidComponent.ID.fromStr(entity["Entity"].as<std::string>().c_str());

				GetIfExists<IdentityComponent>(entity, deserializedEntity);
				GetIfExists<TransformComponent>(entity, deserializedEntity);
				
				SpriteComponent* spriteComponent = GetIfExists<SpriteComponent>(entity, deserializedEntity);
				if (spriteComponent && spriteComponent->HasSpriteSheet())
					spriteComponent->GenerateSpriteSheet();
				
				GetIfExists<CameraComponent>(entity, deserializedEntity);
				GetIfExists<ScriptComponent>(entity, deserializedEntity);
				GetIfExists<PhysicsBodyComponent>(entity, deserializedEntity);

				auto particleNode = entity[typeid(ParticleComponent).name()];
				if (particleNode)
				{
					YAMLDeserializer deserializer(particleNode);
					if (deserializedEntity.HasComponent<ParticleComponent>())
					{
						ParticleComponent& component = deserializedEntity.GetComponent<ParticleComponent>();
						deserializer.Deserialize(component.ParticleSystem);
						deserializer.Deserialize(component.Particle);
					}
					else
					{
						ParticleComponent& component = deserializedEntity.AddComponent<ParticleComponent>();
						deserializer.Deserialize(component.ParticleSystem);
						deserializer.Deserialize(component.Particle);
					}
				}
			}
		}
	}
}