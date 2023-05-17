#include "mpch.h"
#include "Scripting/ScriptEngine.h"
#include "Scripting/ScriptDepot.h"

#include "Engine/Entity.h"
#include "Engine/Components.h"

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"


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

	struct ScriptEngineData
	{
		MonoDomain* RootDomain = nullptr;
		MonoDomain* AppDomain = nullptr;

		MonoAssembly* ScripterAssembly = nullptr;
		MonoImage* ScripterImage = nullptr;

		ScriptClass EntityClass;

		std::unordered_map<std::string, Shared<ScriptClass>> EntityClasses;
		std::unordered_map<std::string, Shared<ScriptInstance>> EntityInstances;

		Scene* RuntimeScene;
	};

	static ScriptEngineData* s_Data = nullptr;

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData();

		InitMono();
		LoadAssembly("Resource/Scripts/MoonScripter.dll");
		LoadAssemblyClasses(s_Data->ScripterAssembly);

		ScriptDepot::InitializeScripts();
		ScriptDepot::RegisterComponents();

		s_Data->EntityClass = ScriptClass("MoonEngine", "Entity");
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

	void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
	{
		s_Data->EntityClasses.clear();

		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		MonoClass* entityClass = mono_class_from_name(s_Data->ScripterImage, "MoonEngine", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			MonoClass* monoClass = mono_class_from_name(s_Data->ScripterImage, nameSpace, name);

			if (monoClass == entityClass)
				continue;

			std::string fullName = "";
			if (nameSpace != "")
				fullName = fmt::format("{}.{}", nameSpace, name);
			else
				fullName = name;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

			if (isEntity)
				s_Data->EntityClasses[fullName] = MakeShared<ScriptClass>(nameSpace, name);

			ME_SYS_LOG("Assembly Types: {}.{}", nameSpace, name);
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
			Shared<ScriptInstance> instance = MakeShared<ScriptInstance>(s_Data->EntityClasses[scriptComponent.ClassName], entity);
			s_Data->EntityInstances[entity.GetUUID()] = instance;

			instance->InvokeAwake();
		}
	}

	void ScriptEngine::UpdateEntity(Entity entity, ScriptComponent& scriptComponent, float dt)
	{
		const std::string& uuid = entity.GetUUID();

		ME_ASSERT((s_Data->EntityInstances.find(uuid) != s_Data->EntityInstances.end()), "Calling an entity that s no Instance!");

		Shared<ScriptInstance> instance = s_Data->EntityInstances[uuid];
		instance->InvokeUpdate(dt);
	}

	std::unordered_map<std::string, Shared<ScriptClass>> MoonEngine::ScriptEngine::GetEntityClasses()
	{
		return s_Data->EntityClasses;
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

	ScriptClass::ScriptClass(const std::string& nspace, const std::string& className)
		:m_Namespace(nspace), m_ClassName(className)
	{
		m_MonoClass = mono_class_from_name(s_Data->ScripterImage, nspace.c_str(), className.c_str());
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
		int entityID = (int)entity.m_ID;
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
}
