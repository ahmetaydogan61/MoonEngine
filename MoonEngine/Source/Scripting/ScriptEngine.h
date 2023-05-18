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

		template<typename T>
		T GetFieldValue(const std::string& name)
		{
			bool foundValue = GetFieldValueInternal(name, s_FieldValueBuffer);
			if (!foundValue)
				return T();

			return *(T*)s_FieldValueBuffer;
		}

		template<typename T>
		void SetFieldValue(const std::string& name, const T& value)
		{
			bool foundValue = SetFieldValueInternal(name, &value);
		}
	private:
		Shared<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_AwakeMethod = nullptr;
		MonoMethod* m_UpdateMethod = nullptr;

		bool GetFieldValueInternal(const std::string& name, void* valueBuffer);
		bool SetFieldValueInternal(const std::string& name, const void* valueBuffer);

		inline static char s_FieldValueBuffer[8];
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