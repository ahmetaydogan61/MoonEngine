#include "mpch.h"
#include "Engine/Tools/SceneSerializer.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/UUID.h"
#include "Engine/Scene.h"

#include "Scripting/ScriptEngine.h"

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

	template<>
	struct convert<MoonEngine::UUID>
	{
		static Node encode(MoonEngine::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, MoonEngine::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};
}

namespace MoonEngine
{
#define SERIALIZE_FIELD(FieldType, Type) case ScriptFieldType::FieldType: out << *(Type*)field.Data; break;

#define DESERIALIZE_FIELD(FieldType, Type) case ScriptFieldType::FieldType: {\
		Type value = scriptField["Value"].as<Type>();\
		memcpy(field.Data, &value, sizeof(Type));\
		break; }

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
	std::string GetTypeName()
	{
		std::string typeName = typeid(T).name();
		size_t pos = typeName.find_last_of(":");
		return typeName.substr(pos + 1);
	}

	template<typename T>
	void SerializeIfExists(YAML::Emitter& out, Entity& entity)
	{
		if (entity.HasComponent<T>())
		{
			YAMLSerializer parser(out);
			T& c = entity.GetComponent<T>();
			parser.BeginParse(GetTypeName<T>());
			parser.Serialize(c);
			parser.EndParse();
		}
	}

	void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;
		const auto& uuid = entity.GetComponent<UUIDComponent>();
		out << YAML::Key << "Entity" << YAML::Value << uuid.ID;

		SerializeIfExists<IdentityComponent>(out, entity);
		SerializeIfExists<TransformComponent>(out, entity);
		SerializeIfExists<CameraComponent>(out, entity);
		SerializeIfExists<SpriteComponent>(out, entity);

		if (entity.HasComponent<ScriptComponent>())
		{
			YAMLSerializer parser(out);
			ScriptComponent& c = entity.GetComponent<ScriptComponent>();
			parser.BeginParse(GetTypeName<ScriptComponent>());
			parser.Serialize(c);

			auto scriptInstance = ScriptEngine::GetScriptInstance(uuid.ID);
			if (scriptInstance && scriptInstance->GetInstanceFields().size() > 0)
			{
				out << YAML::Key << "Fields" << YAML::Value;
				out << YAML::BeginSeq;

				for (const auto& [name, field] : scriptInstance->GetInstanceFields())
				{
					if (!scriptInstance->GetScriptClass()->GetFields().contains(name))
						continue;

					out << YAML::BeginMap;
					out << YAML::Key << "Name" << YAML::Value << name;
					out << YAML::Key << "Type" << YAML::Value << ScriptFieldTypeConverter::ToString(field.Type);
					out << YAML::Key << "Value" << YAML::Value;

					switch (field.Type)
					{
						SERIALIZE_FIELD(Char, char);
						SERIALIZE_FIELD(Bool, bool);

						SERIALIZE_FIELD(Float, float);
						SERIALIZE_FIELD(Double, double);

						SERIALIZE_FIELD(Byte, int8_t);
						SERIALIZE_FIELD(Short, int16_t);
						SERIALIZE_FIELD(Int, int32_t);
						SERIALIZE_FIELD(Long, int64_t);

						SERIALIZE_FIELD(UByte, uint8_t);
						SERIALIZE_FIELD(UShort, uint16_t);
						SERIALIZE_FIELD(UInt, uint32_t);
						SERIALIZE_FIELD(ULong, uint64_t);

						SERIALIZE_FIELD(Vector2, glm::vec2);
						SERIALIZE_FIELD(Vector3, glm::vec3);
						SERIALIZE_FIELD(Vector4, glm::vec4);

						SERIALIZE_FIELD(Entity, UUID);
					}

					out << YAML::EndMap;
				}

				out << YAML::EndSeq;
			}
			parser.EndParse();
		}

		SerializeIfExists<PhysicsBodyComponent>(out, entity);

		if (entity.HasComponent<ParticleComponent>())
		{
			YAMLSerializer parser(out);
			ParticleComponent& c = entity.GetComponent<ParticleComponent>();
			parser.BeginParse(GetTypeName<ParticleComponent>());
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
		auto componentNode = node[GetTypeName<T>()];
		if (componentNode)
		{
			YAMLDeserializer deserializer(componentNode);
			T& component = entity.HasComponent<T>() ? entity.GetComponent<T>() : entity.AddComponent<T>();
			deserializer.Deserialize(component);
			return &component;
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
				UUID uuid(entity["Entity"].as<uint64_t>());

				Entity deserializedEntity = scene->CreateEntity(uuid);

				GetIfExists<IdentityComponent>(entity, deserializedEntity);
				GetIfExists<TransformComponent>(entity, deserializedEntity);

				SpriteComponent* spriteComponent = GetIfExists<SpriteComponent>(entity, deserializedEntity);
				if (spriteComponent && spriteComponent->HasSpriteSheet())
					spriteComponent->GenerateSpriteSheet();

				GetIfExists<CameraComponent>(entity, deserializedEntity);
				GetIfExists<PhysicsBodyComponent>(entity, deserializedEntity);

				auto scriptNode = entity[GetTypeName<ScriptComponent>()];
				if (scriptNode)
				{
					YAMLDeserializer deserializer(scriptNode);
					ScriptComponent component = {};
					component.ClassName = scriptNode["ClassName"].as<std::string>();

					deserializedEntity.ReplaceComponent<ScriptComponent>(component);
					auto scriptInstance = ScriptEngine::GetScriptInstance(uuid);

					auto scriptFields = scriptNode["Fields"];
					if (scriptFields)
					{
						ME_LOG("Script Field Exists");
						auto scriptInstance = ScriptEngine::GetScriptInstance(uuid);
						if (scriptInstance)
						{
							auto& instanceFields = scriptInstance->GetInstanceFields();
							for (const auto& scriptField : scriptFields)
							{
								const auto& name = scriptField["Name"].as<std::string>();

								if (!instanceFields.contains(name))
									continue;

								ScriptField& field = instanceFields.at(name);
								ME_LOG(field.FieldName);

								switch (field.Type)
								{
									DESERIALIZE_FIELD(Char, char);
									DESERIALIZE_FIELD(Bool, bool);

									DESERIALIZE_FIELD(Float, float);
									DESERIALIZE_FIELD(Double, double);

									DESERIALIZE_FIELD(Byte, int8_t);
									DESERIALIZE_FIELD(Short, int16_t);
									DESERIALIZE_FIELD(Int, int32_t);
									DESERIALIZE_FIELD(Long, int64_t);

									DESERIALIZE_FIELD(UByte, uint8_t);
									DESERIALIZE_FIELD(UShort, uint16_t);
									DESERIALIZE_FIELD(UInt, uint32_t);
									DESERIALIZE_FIELD(ULong, uint64_t);

									case ScriptFieldType::Vector2:
									{
										glm::vec2 value = glm::make_vec2(scriptField["Value"].as<std::vector<float>>().data());
										memcpy(field.Data, &value, sizeof(glm::vec2));
										break;
									}
									case ScriptFieldType::Vector3:
									{
										glm::vec3 value = glm::make_vec3(scriptField["Value"].as<std::vector<float>>().data());
										memcpy(field.Data, &value, sizeof(glm::vec3));
										break;
									}
									case ScriptFieldType::Vector4:
									{
										glm::vec4 value = glm::make_vec4(scriptField["Value"].as<std::vector<float>>().data());
										memcpy(field.Data, &value, sizeof(glm::vec4));
										break;
									}

									DESERIALIZE_FIELD(Entity, UUID);
								}

							}
						}
					}
				}

				auto particleNode = entity[GetTypeName<ParticleComponent>()];
				if (particleNode)
				{
					YAMLDeserializer deserializer(particleNode);
					ParticleComponent& component = deserializedEntity.HasComponent<ParticleComponent>() ?
						deserializedEntity.GetComponent<ParticleComponent>() : deserializedEntity.AddComponent<ParticleComponent>();

					deserializer.Deserialize(component.ParticleSystem);
					deserializer.Deserialize(component.Particle);
				}
			}
		}
	}
}