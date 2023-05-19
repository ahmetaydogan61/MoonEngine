#include "mpch.h"
#include "Scripting/ScriptEngine.h"
#include "Scripting/ScriptDepot.h"

#include "Engine/Entity.h"
#include "Engine/Components.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"

namespace MoonEngine
{
	static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			ME_SYS_ERR("Assembly Path Not Found!");
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
			return nullptr;

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = (uint32_t)size;
		return buffer;
	}

	static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			return nullptr;
		}

		std::string asmPath = assemblyPath.string();
		MonoAssembly* assembly = mono_assembly_load_from_full(image, asmPath.c_str(), &status, 0);
		mono_image_close(image);

		delete[] fileData;

		return assembly;
	}

	void PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

			ME_SYS_LOG("Assembly Types: {}.{}", nameSpace, name);
		}
	}

	static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
	{
		{ "System.Boolean", ScriptFieldType::Bool },
		{ "System.Single", ScriptFieldType::Float },
		{ "System.Double", ScriptFieldType::Double },

		{ "System.Byte", ScriptFieldType::Byte },
		{ "System.Char", ScriptFieldType::Char },
		{ "System.Int16", ScriptFieldType::Short },
		{ "System.Int32", ScriptFieldType::Int },
		{ "System.Int64", ScriptFieldType::Long },

		{ "System.UByte", ScriptFieldType::UByte },
		{ "System.UInt16", ScriptFieldType::UShort },
		{ "System.UInt32", ScriptFieldType::UInt },
		{ "System.UInt64", ScriptFieldType::ULong },

		{ "MoonEngine.Vector2", ScriptFieldType::Vector2 },
		{ "MoonEngine.Vector3", ScriptFieldType::Vector3 },
		{ "MoonEngine.Vector4", ScriptFieldType::Vector4 },

		{ "MoonEngine.Entity", ScriptFieldType::Entity},
	};

	ScriptFieldType MonoTypeToScriptFieldType(MonoType* type)
	{
		std::string typeName = mono_type_get_name(type);

		auto it = s_ScriptFieldTypeMap.find(typeName);
		if (it == s_ScriptFieldTypeMap.end())
		{
			ME_SYS_ERR("Unknown Type: {}", typeName);
			return ScriptFieldType::None;
		}

		return it->second;
	}

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* ScripterAssembly = nullptr;
		MonoImage* ScripterImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Shared<ScriptClass>> EntityClasses;
		std::unordered_map<UUID, Shared<ScriptInstance>> EntityInstances;

		std::unordered_map<UUID, ScriptFieldMap> EntityScriptFields;

		Scene* RuntimeScene;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		LoadAssembly("Resource/Scripts/MoonScripter.dll");
		LoadAppAssembly("TemplateProject/Build/Template.dll");
		LoadAssemblyClasses();

		ScriptDepot::InitializeScripts();
		ScriptDepot::RegisterComponents();

		s_Data->EntityClass = ScriptClass("MoonEngine", "Entity", true);
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib");

		MonoDomain* rootDomain = mono_jit_init("MoonJitRuntime");
		ME_ASSERT(rootDomain, "Mono Domain Could Not Created!");
		s_Data->RootDomain = rootDomain;
	}

	void ScriptEngine::LoadAssembly(const std::filesystem::path& path)
	{
		s_Data->AppDomain = mono_domain_create_appdomain(std::string("MoonScriptRuntime").data(), nullptr);
		mono_domain_set(s_Data->AppDomain, true);

		s_Data->ScripterAssembly = LoadMonoAssembly(path);
		s_Data->ScripterImage = mono_assembly_get_image(s_Data->ScripterAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& path)
	{
		s_Data->AppAssembly = LoadMonoAssembly(path);
		s_Data->AppImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->EntityClasses.clear();

		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* entityClass = mono_class_from_name(s_Data->ScripterImage, "MoonEngine", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* monoClass = mono_class_from_name(s_Data->AppImage, nameSpace, className);

			if (monoClass == entityClass)
				continue;

			std::string fullName = "";
			if (nameSpace != "")
				fullName = fmt::format("{}.{}", nameSpace, className);
			else
				fullName = className;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

			if (!isEntity)
				continue;

			Shared<ScriptClass> scriptClass = MakeShared<ScriptClass>(nameSpace, className);
			s_Data->EntityClasses[fullName] = scriptClass;

			int fieldCount = mono_class_num_fields(monoClass);
			void* iterator = nullptr;

			ME_SYS_LOG("{} Fields Of '{}'", fieldCount, className);
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				const char* fieldName = mono_field_get_name(field);
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					MonoType* type = mono_field_get_type(field);
					ScriptFieldType fieldType = MonoTypeToScriptFieldType(type);
					const char* typeName = ScriptFieldTypeToString(fieldType);

					ME_SYS_LOG("Name: {} - Type: {}", fieldName, typeName);

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}

			ME_SYS_LOG("Assembly Types: {}.{}", nameSpace, className);
		}

		auto& entityClasses = s_Data->EntityClasses;

	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::ShutdownMono()
	{
		//mono_domain_unload(s_Data->AppDomain);
		s_Data->AppDomain = nullptr;

		mono_jit_cleanup(s_Data->RootDomain);
		s_Data->RootDomain = nullptr;
	}

	void ScriptEngine::StartRuntime(Scene* scene)
	{
		s_Data->RuntimeScene = scene;
	}

	void ScriptEngine::StopRuntime()
	{
		s_Data->RuntimeScene = nullptr;
		s_Data->EntityInstances.clear();
	}

	bool ScriptEngine::CheckEntityClass(const std::string& fullName)
	{
		auto& entityClasses = s_Data->EntityClasses;
		return entityClasses.find(fullName) != entityClasses.end();
	}

	void ScriptEngine::CreateEntity(Entity entity, ScriptComponent& scriptComponent)
	{
		if (CheckEntityClass(scriptComponent.ClassName))
		{
			const auto& uuid = entity.GetUUID();
			Shared<ScriptInstance> instance = MakeShared<ScriptInstance>(s_Data->EntityClasses[scriptComponent.ClassName], entity);
			s_Data->EntityInstances[uuid] = instance;

			if (s_Data->EntityScriptFields.find(uuid) != s_Data->EntityScriptFields.end())
			{
				const ScriptFieldMap& fieldMap = s_Data->EntityScriptFields.at(uuid);

				for (const auto& [name, fieldInstance] : fieldMap)
					instance->SetFieldValue(name, fieldInstance.GetData());
			}

			instance->InvokeAwake();
		}
	}

	void ScriptEngine::UpdateEntity(Entity entity, ScriptComponent& scriptComponent, float dt)
	{
		UUID uuid = entity.GetUUID();

		ME_ASSERT((s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end()), "Calling an entity that s no Instance!");

		Shared<ScriptInstance> instance = s_Data->EntityInstances[uuid];
		instance->InvokeUpdate(dt);
	}

	std::unordered_map<std::string, Shared<ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
	}

	Shared<ScriptClass> ScriptEngine::GetEntityClass(const std::string& name)
	{
		if (s_Data->EntityClasses.find(name) == s_Data->EntityClasses.end())
			return nullptr;

		return s_Data->EntityClasses.at(name);
	}

	ScriptFieldMap& ScriptEngine::GetScriptFieldMap(Entity e)
	{
		ME_ASSERT(e, "Entity is null!");

		UUID uuid = e.GetUUID();
		return s_Data->EntityScriptFields[uuid];
	}

	Shared<ScriptInstance> ScriptEngine::GetEntityScriptInstance(UUID entityId)
	{
		auto it = s_Data->EntityInstances.find(entityId);
		if (it == s_Data->EntityInstances.end())
			return nullptr;

		return it->second;
	}

	MonoObject* ScriptEngine::GetMonoInstance(UUID uuid)
	{
		ME_ASSERT((s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end()), "Mono Object not found!");
		return s_Data->EntityInstances.at(uuid)->GetMonoObject();
	}

	MonoImage* ScriptEngine::GetScripterImage()
	{
		return s_Data->ScripterImage;
	}

	Scene* ScriptEngine::GetRuntimeScene()
	{
		return s_Data->RuntimeScene;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
		mono_runtime_object_init(instance);
		return instance;
	}

	ScriptClass::ScriptClass(const std::string& nspace, const std::string& className, bool fromScripter)
		:m_Namespace(nspace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(fromScripter ? s_Data->ScripterImage : s_Data->AppImage, nspace.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_MonoClass);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, instance, params, nullptr);
	}

	ScriptInstance::ScriptInstance(Shared<ScriptClass> scriptClass, Entity entity)
		:m_ScriptClass(scriptClass)
	{
		m_Instance = scriptClass->Instantiate();

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_AwakeMethod = scriptClass->GetMethod("Awake", 0);
		m_UpdateMethod = scriptClass->GetMethod("Update", 1);

		//Constructor Call
		UUID entityID = entity.GetUUID();
		void* param = &entityID;
		m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
	}

	void ScriptInstance::InvokeAwake()
	{
		if (m_AwakeMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_AwakeMethod);
	}

	void ScriptInstance::InvokeUpdate(float dt)
	{
		if (m_UpdateMethod)
		{
			void* param = &dt;
			m_ScriptClass->InvokeMethod(m_Instance, m_UpdateMethod, &param);
		}
	}

	bool ScriptInstance::GetFieldValue(const std::string& name, void* valueBuffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);

		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;

		mono_field_get_value(m_Instance, field.MonoField, valueBuffer);
		return true;
	}

	bool ScriptInstance::SetFieldValue(const std::string& name, const void* valueBuffer)
	{
		const auto& fields = m_ScriptClass->GetFields();
		auto it = fields.find(name);

		if (it == fields.end())
			return false;

		const ScriptField& field = it->second;

		mono_field_set_value(m_Instance, field.MonoField, (void*)valueBuffer);
		return true;
	}
}
