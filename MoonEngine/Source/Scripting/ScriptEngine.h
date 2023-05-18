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
	struct ScriptComponent;

	enum class ScriptFieldType
	{
		None = 0,
		Bool, Float, Double,
		Byte, Char, Short, Int, Long,
		UByte, UShort, UInt, ULong,
		Vector2, Vector3, Vector4,
		Entity, TransformComponent, PhysicsBodyComponent
	};

	struct ScriptField
	{
		ScriptFieldType Type;
		std::string Name;
		MonoClassField* MonoField;
	};

	struct ScriptFieldInstance
	{
		ScriptField Field;

		ScriptFieldInstance()
		{
			memset(m_Data, 0, sizeof(m_Data));
		}

		template<typename T>
		T GetValue() 
		{ 
			static_assert(sizeof(T) <= 8, "Type to large!");
			return *(T*)m_Data;
		}

		template<typename T>
		void SetValue(T valueBuffer)
		{
			static_assert(sizeof(T) <= 8, "Type to large!");
			memcpy(m_Data, &valueBuffer, sizeof(T));
		}

		const uint8_t* GetData() const { return m_Data; }
	private:
		uint8_t m_Data[8];
	};

	using ScriptFieldMap = std::unordered_map<std::string, ScriptFieldInstance>;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& nspace, const std::string& className, bool fromScripter = false);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

		const std::map<std::string, ScriptField>& GetFields() const { return m_Fields; }
	private:
		std::string m_Namespace;
		std::string m_ClassName;

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

		Shared<ScriptClass> GetScriptClass() { return m_ScriptClass; }

		bool GetFieldValue(const std::string& name, void* valueBuffer);
		bool SetFieldValue(const std::string& name, const void* valueBuffer);
	private:
		Shared<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
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

		static void CreateEntity(Entity entity, ScriptComponent& scriptComponent);
		static void UpdateEntity(Entity entity, ScriptComponent& scriptComponent, float dt);

		static void StartRuntime(Scene* scene);
		static void StopRuntime();

		static bool CheckEntityClass(const std::string& fullName);
		static std::unordered_map<std::string, Shared<ScriptClass>> GetEntityClasses();
		static Shared<ScriptClass> GetEntityClass(const std::string& name);
		static ScriptFieldMap& GetScriptFieldMap(Entity entity);
		static Shared<ScriptInstance> GetEntityScriptInstance(const std::string& entityId);

		static MonoImage* GetScripterImage();
		static Scene* GetRuntimeScene();
	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses();

		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend class ScriptClass;
	};
}