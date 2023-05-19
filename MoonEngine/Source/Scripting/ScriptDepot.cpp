#include "mpch.h"
#include "Scripting/ScriptDepot.h"
#include "Scripting/ScriptEngine.h"

#include "Core/Input.h"

#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "mono/metadata/object.h"
#include <mono/metadata/reflection.h>

namespace MoonEngine
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define ME_ADD_INTERNAL_CALL(name) mono_add_internal_call("MoonEngine.InternalCalls::" #name, name)

	static MonoObject* GetScriptInstance(UUID entityId)
	{
		return ScriptEngine::GetMonoInstance(entityId);
	}

	static bool Entity_HasComponent(UUID entityId, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		ME_ASSERT(scene, "Scene Not Found!");

		auto entity = scene->FindEntityWithUUID(entityId);
		ME_ASSERT(entity, "Entity Does not Exist!");

		MonoType* monoType = mono_reflection_type_get_type(componentType);
		ME_ASSERT((s_EntityHasComponentFuncs.find(monoType) != s_EntityHasComponentFuncs.end()), "Has not Mono Type!");

		return s_EntityHasComponentFuncs.at(monoType)(entity);
	}

	static uint64_t Entity_FindByName(MonoString* name)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		ME_ASSERT(scene, "Scene Not Found!");

		char* nameStr = mono_string_to_utf8(name);
		Entity entity = scene->FindEntityWithName(nameStr);
		mono_free(nameStr);

		if (!entity)
			return 0;

		return entity.GetUUID();
	}

	static void Transform_SetPosition(UUID entityId, glm::vec3* refPosition)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<TransformComponent>().Position = *refPosition;
	}

	static void Transform_GetPosition(UUID entityId, glm::vec3* outPosition)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static void PhysicsBody_AddForce(UUID entityId, glm::vec2* force, glm::vec2* position)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);

		entity.GetComponent<PhysicsBodyComponent>().AddForce(*force, *position);
	}

	static bool Input_GetKey(Keycode keycode)
	{
		return Input::GetKey(keycode);
	}

	template<typename... T>
	static void RegisterComponent()
	{
		([]()
		{
			std::string_view typeName = typeid(T).name();

			size_t pos = typeName.find_last_of(":");
			std::string_view componentName = typeName.substr(pos + 1);
			std::string managedTypeName = fmt::format("MoonEngine.{0}", componentName);

			MonoType* managedType = mono_reflection_type_from_name(managedTypeName.data(), ScriptEngine::GetScripterImage());
			if (!managedType)
			{
				ME_SYS_ERR("Component Type not found: {}", managedTypeName);
				return;
			}

			s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<T>(); };
		}(), ...);
	}

	template<typename... T>
	static void RegisterComponent(ComponentGroup<T...>)
	{
		RegisterComponent<T...>();
	}

	void ScriptDepot::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptDepot::InitializeScripts()
	{
		ME_ADD_INTERNAL_CALL(GetScriptInstance);

		ME_ADD_INTERNAL_CALL(Entity_HasComponent);
		ME_ADD_INTERNAL_CALL(Entity_FindByName);

		ME_ADD_INTERNAL_CALL(Transform_SetPosition);
		ME_ADD_INTERNAL_CALL(Transform_GetPosition);

		ME_ADD_INTERNAL_CALL(PhysicsBody_AddForce);

		ME_ADD_INTERNAL_CALL(Input_GetKey);
	}
}
