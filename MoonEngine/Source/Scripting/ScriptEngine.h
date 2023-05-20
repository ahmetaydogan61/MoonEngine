#pragma once

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoClassField MonoClassField;
}

namespace MoonEngine
{
	class Entity;
	class Scene;
	class UUID;

	enum class ScriptFieldType
	{
		Unknown = 0,
		Float,
		Entity
	};

	struct ScriptFieldConverter
	{
		static const char* ToString(ScriptFieldType fieldType);
		static ScriptFieldType FromString(std::string_view fieldName);
		static ScriptFieldType MonoToType(MonoClassField* monoField);
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string FieldName;
		MonoClassField* MonoField;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& nameSpace, const std::string className);
		ScriptClass(const std::string& nameSpace, const std::string className, MonoClass* monoClass);

		MonoClass* GetMonoClass() const { return m_MonoClass; }
		MonoMethod* GetMethod(const std::string& name, int parameterCount);

		std::string GetName() const { return m_Name; }
		std::string GetNameSpace() const { return m_Namespace; }
		std::string GetFullname() const { return fmt::format("{}.{}", m_Namespace, m_Name); }

		const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }

	private:
		std::string m_Namespace;
		std::string m_Name;

		std::map<std::string, ScriptField> m_Fields;

		MonoClass* m_MonoClass = nullptr;

		friend class ScriptEngine;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Shared<ScriptClass> scriptClass, Entity entity);

		void InvokeAwake();
		void InvokeUpdate(float dt);
		MonoObject* InvokeMethod(MonoMethod* method, void** params = nullptr);

		Shared<ScriptClass> GetScriptClass() { return m_ScriptClass; }
		MonoObject* GetMonoObject() { return m_Instance; }

		bool GetFieldValue(const ScriptField& field, void* value);
		bool SetFieldValue(const ScriptField& field, const void* value);

		void SetEntityReference(const ScriptField& field, Entity entity, uint64_t value);
		uint64_t GetEntityReference(const ScriptField& field, Entity entity);
	private:
		Shared<ScriptClass> m_ScriptClass;
		MonoObject* m_Instance = nullptr;

		//Methods
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_AwakeMethod = nullptr;
		MonoMethod* m_UpdateMethod = nullptr;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& path);
		static void LoadAppAssembly(const std::filesystem::path& path);

		static MonoImage* GetScripterImage();

		static void StartRuntime(Scene* scene);
		static void StopRuntime();
		static Scene* GetRuntimeScene();

		static bool CheckScriptClass(const std::string& scriptName);
		static Shared<ScriptClass> GetScriptClass(const std::string& className);
		static Shared<ScriptInstance> GetScriptInstance(UUID id);

		static void AwakeEntity(Entity entity, const std::string& scriptName);
		static void UpdateEntity(Entity entity, const std::string& scriptName, float dt);
	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses();

		friend class ScriptClass;
	};
}