#pragma once

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
}

namespace MoonEngine
{
	class Entity;
	class Scene;
	struct ScriptComponent;

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& nspace, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& name, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);
	private:
		std::string m_Namespace;
		std::string m_ClassName;
		MonoClass* m_MonoClass = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(Shared<ScriptClass> scriptClass, Entity entity);

		void InvokeAwake();
		void InvokeUpdate(float dt);
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

		static void CreateEntity(Entity entity, ScriptComponent& scriptComponent);
		static void UpdateEntity(Entity entity, ScriptComponent& scriptComponent, float dt);

		static void StartRuntime(Scene* scene);
		static void StopRuntime();

		static bool CheckEntityClass(const std::string& fullName);
		static std::unordered_map<std::string, Shared<ScriptClass>> GetEntityClasses();

		static MonoImage* GetScripterImage();
		static Scene* GetRuntimeScene();
	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadAssemblyClasses(MonoAssembly* assembly);

		static MonoObject* InstantiateClass(MonoClass* monoClass);


		friend class ScriptClass;
	};
}