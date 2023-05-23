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
		Byte, Char, Bool,
		Float, Double,
		Short, Int, Long,
		UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity
	};

	struct ScriptFieldTypeConverter
	{
		static const char* ToString(ScriptFieldType fieldType);
		static ScriptFieldType FromString(std::string_view fieldName);
		static ScriptFieldType FromMonoType(MonoClassField* monoField);
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string FieldName;
		MonoClassField* MonoField;
	
		uint8_t Data[16];
		char* stringData = nullptr;
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

		MonoClass* m_MonoClass = nullptr;
		std::map<std::string, ScriptField> m_Fields;

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

		void GetEntityReference(const ScriptField& field, void* value);
		void SetEntityReference(const ScriptField& field, const void* value);

		std::map<std::string, ScriptField>& GetInstanceFields() { return m_InstanceFields; }
		void SetInstanceFields(const std::map<std::string, ScriptField>& instanceFields) { m_InstanceFields = instanceFields; }
		void CopyInstanceFields(std::map<std::string, ScriptField>& instanceFields);
	private:
		Shared<ScriptClass> m_ScriptClass;
		MonoObject* m_Instance = nullptr;

		std::map<std::string, ScriptField> m_InstanceFields;

		//Methods
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_AwakeMethod = nullptr;
		MonoMethod* m_UpdateMethod = nullptr;

		friend class ScriptEngine;
	};

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::filesystem::path& path);
		static void LoadAppAssembly(const std::filesystem::path& path);
		static void ReloadAssembly();

		static MonoImage* GetScripterImage();

		static void SetRuntimeScene(Scene* scene);
		static Scene* GetRuntimeScene();

		static Shared<ScriptInstance> CreateEntityInstance(Entity entity, const std::string& scriptName);
		static void AwakeEntity(Entity entity, const std::string& scriptName);
		static void UpdateEntity(Entity entity, const std::string& scriptName, float dt);
		static void DestroyEntity(Entity entity);

		static bool CheckScriptClass(const std::string& scriptName);
		static Shared<ScriptClass> GetScriptClass(const std::string& className);
		static const std::unordered_map<std::string, Shared<ScriptClass>>& GetScriptClasses();

		static bool CheckScriptInstance(UUID id);
		static Shared<ScriptInstance> GetScriptInstance(UUID id);
		static const std::unordered_map<UUID, Shared<ScriptInstance>>& GetScriptInstances();
		static void ClearScriptInstances();
	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses();

		friend class ScriptClass;
	};
}