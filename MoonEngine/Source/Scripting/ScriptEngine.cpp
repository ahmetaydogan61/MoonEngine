#include "mpch.h"
#include "Scripting/ScriptEngine.h"
#include "Scripting/ScriptDepot.h"
#include "Scripting/ScriptingUtils.h"

#include "Engine/Entity.h"
#include "Engine/Components.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"
#include "mono/metadata/tabledefs.h"

namespace MoonEngine
{
	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* ScripterAssembly = nullptr;
		MonoImage* ScripterImage = nullptr;

		MonoAssembly* AppAssembly = nullptr;
		MonoImage* AppImage = nullptr;

		ScriptClass EntityClass;

		//<Class Fullname, Script Class> 
		std::unordered_map<std::string, Shared<ScriptClass>> ScriptClasses;

		std::unordered_map<UUID, Shared<ScriptInstance>> ScriptInstances;

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

		s_Data->ScripterAssembly = ScriptingUtils::LoadMonoAssembly(path);
		MonoAssembly* assm = s_Data->ScripterAssembly;

		s_Data->ScripterImage = mono_assembly_get_image(s_Data->ScripterAssembly);
	}

	void ScriptEngine::LoadAppAssembly(const std::filesystem::path& path)
	{
		s_Data->AppAssembly = ScriptingUtils::LoadMonoAssembly(path);
		s_Data->AppImage = mono_assembly_get_image(s_Data->AppAssembly);
	}

	void ScriptEngine::LoadAssemblyClasses()
	{
		s_Data->ScriptClasses.clear();

		MonoClass* entityClass = mono_class_from_name(s_Data->ScripterImage, "MoonEngine", "Entity");

		int fieldCount = mono_class_num_fields(entityClass);
		void* iterator = nullptr;

		s_Data->EntityClass = { "MoonEngine", "Entity", entityClass };

		while (MonoClassField* field = mono_class_get_fields(entityClass, &iterator))
		{
			uint32_t flags = mono_field_get_flags(field);
			if (flags & FIELD_ATTRIBUTE_PUBLIC)
			{
				const char* fieldName = mono_field_get_name(field);
				ScriptFieldType fieldType = ScriptFieldConverter::MonoToType(field);

				const char* typeName = ScriptFieldConverter::ToString(fieldType);

				ME_SYS_LOG("Name: {} - Type: {}", fieldName, typeName);

				s_Data->EntityClass.m_Fields[fieldName] = { fieldType, fieldName, field };
			}
		}


		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(s_Data->AppImage, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(s_Data->AppImage, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* className = mono_metadata_string_heap(s_Data->AppImage, cols[MONO_TYPEDEF_NAME]);

			MonoClass* monoClass = mono_class_from_name(s_Data->AppImage, nameSpace, className);

			if (monoClass == entityClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (!isEntity)
				continue;

			Shared<ScriptClass> scriptClass = MakeShared<ScriptClass>(nameSpace, className);
			s_Data->ScriptClasses[scriptClass->GetFullname()] = scriptClass;

			ME_SYS_LOG("Class (Namespace.Name): {}", scriptClass->GetFullname());
			ME_SYS_LOG("Class (Namespace.Name): {}", mono_class_get_name(monoClass));

			int fieldCount = mono_class_num_fields(monoClass);
			void* iterator = nullptr;

			ME_SYS_LOG("{} Fields Of '{}'", fieldCount, className);
			while (MonoClassField* field = mono_class_get_fields(monoClass, &iterator))
			{
				uint32_t flags = mono_field_get_flags(field);
				if (flags & FIELD_ATTRIBUTE_PUBLIC)
				{
					const char* fieldName = mono_field_get_name(field);
					ScriptFieldType fieldType = ScriptFieldConverter::MonoToType(field);
					const char* typeName =		ScriptFieldConverter::ToString(fieldType);

					ME_SYS_LOG("Name: {} - Type: {}", fieldName, typeName);

					scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
				}
			}
		}
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

	MonoImage* ScriptEngine::GetScripterImage()
	{
		return s_Data->ScripterImage;
	}

	void ScriptEngine::StartRuntime(Scene* scene)
	{
		s_Data->RuntimeScene = scene;
	}

	void ScriptEngine::StopRuntime()
	{
		s_Data->RuntimeScene = nullptr;
		s_Data->ScriptInstances.clear();
	}

	Scene* ScriptEngine::GetRuntimeScene()
	{
		return s_Data->RuntimeScene;
	}

	bool ScriptEngine::CheckScriptClass(const std::string& scriptName)
	{
		auto& scriptClasses = s_Data->ScriptClasses;
		return scriptClasses.find(scriptName) != scriptClasses.end();
	}

	Shared<ScriptClass> ScriptEngine::GetScriptClass(const std::string& className)
	{
		return s_Data->ScriptClasses.at(className);
	}

	Shared<ScriptInstance> ScriptEngine::GetScriptInstance(UUID id)
	{
		return s_Data->ScriptInstances.at(id);
	}

	void ScriptEngine::AwakeEntity(Entity entity, const std::string& scriptName)
	{
		if (CheckScriptClass(scriptName))
		{
			UUID uuid = entity.GetUUID();
			Shared<ScriptInstance> instance = MakeShared<ScriptInstance>(s_Data->ScriptClasses[scriptName], entity);
			s_Data->ScriptInstances[uuid] = instance;

			instance->InvokeAwake();
		}
		else
			ME_SYS_WAR("Script Class Not Found!");
		//TODO: Console Log, Script Class Not Found!
	}

	void ScriptEngine::UpdateEntity(Entity entity, const std::string& scriptName, float dt)
	{
		if (CheckScriptClass(scriptName))
			s_Data->ScriptInstances.at(entity.GetUUID())->InvokeUpdate(dt);
	}

	ScriptClass::ScriptClass(const std::string& nameSpace, const std::string className)
		:m_Namespace(nameSpace), m_Name(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->AppImage, nameSpace.c_str(), className.c_str());
	}

	ScriptClass::ScriptClass(const std::string& nameSpace, const std::string className, MonoClass* monoClass)
		:m_Namespace(nameSpace), m_Name(className), m_MonoClass(monoClass) {}

	MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount)
	{
		return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
	}

	ScriptInstance::ScriptInstance(Shared<ScriptClass> scriptClass, Entity entity)
		:m_ScriptClass(scriptClass)
	{
		m_Instance = mono_object_new(s_Data->AppDomain, scriptClass->GetMonoClass());
		mono_runtime_object_init(m_Instance);

		m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
		m_AwakeMethod = m_ScriptClass->GetMethod("Awake", 0);
		m_UpdateMethod = m_ScriptClass->GetMethod("Update", 1);

		if (m_Constructor)
		{
			UUID uuid = entity.GetUUID();
			void* param = &uuid;
			InvokeMethod(m_Constructor, &param);
		}
	}

	void ScriptInstance::InvokeAwake()
	{
		if (m_AwakeMethod)
			InvokeMethod(m_AwakeMethod);
	}

	void ScriptInstance::InvokeUpdate(float dt)
	{
		if (m_UpdateMethod)
		{
			void* param = &dt;
			InvokeMethod(m_UpdateMethod, &param);
		}
	}

	MonoObject* ScriptInstance::InvokeMethod(MonoMethod* method, void** params)
	{
		return mono_runtime_invoke(method, m_Instance, params, nullptr);
	}
	
	bool ScriptInstance::GetFieldValue(const ScriptField& field, void* value)
	{
		if (field.Type == ScriptFieldType::Entity)
		{
			MonoObject* instance = mono_field_get_value_object(s_Data->AppDomain, field.MonoField, m_Instance);

			if(instance)
				mono_field_get_value(instance, s_Data->EntityClass.GetFields().at("ID").MonoField, value);

			return true;
		}

		mono_field_get_value(m_Instance, field.MonoField, value);
		return true;
	}

	bool ScriptInstance::SetFieldValue(const ScriptField& field, const void* value)
	{
		if (field.Type == ScriptFieldType::Entity)
		{
			uint64_t castedValue = *(uint64_t*)value;

			if (!s_Data->ScriptInstances.contains(castedValue))
				return false;

			mono_field_set_value(m_Instance, field.MonoField, (void*)s_Data->ScriptInstances.at(castedValue)->GetMonoObject());
			return true;
		}

		mono_field_set_value(m_Instance, field.MonoField, (void*)value);
		return true;
	}

	const char* ScriptFieldConverter::ToString(ScriptFieldType fieldType)
	{
		switch (fieldType)
		{
			case MoonEngine::ScriptFieldType::Float: return "Float";
			case MoonEngine::ScriptFieldType::Entity: return "Entity";
		}

		ME_SYS_ERR("Unkonw Script Field Type");
		return "Unkonwn";
	}

	ScriptFieldType ScriptFieldConverter::FromString(std::string_view fieldName)
	{
		if (fieldName == "Float") return ScriptFieldType::Float;
		if (fieldName == "Entity") return ScriptFieldType::Entity;

		ME_SYS_ERR("Unkonw Script Field Type");
		return ScriptFieldType::Unknown;
	}

	ScriptFieldType ScriptFieldConverter::MonoToType(MonoClassField* monoField)
	{
		MonoType* type = mono_field_get_type(monoField);
		std::string typeName = mono_type_get_name(type);

		ME_LOG("Type Name: {}", typeName);
		if (typeName == "System.Single") return ScriptFieldType::Float;
		if (typeName == "MoonEngine.Entity") return ScriptFieldType::Entity;

		ME_SYS_ERR("Unknown Script Field Type");
		return ScriptFieldType::Unknown;
	}
}
