#include "mpch.h"
#include "Scripting/ScriptDepot.h"
#include "Scripting/ScriptEngine.h"

#include "Core/Input.h"

#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "mono/metadata/object.h"
#include <mono/metadata/reflection.h>
#include <box2d/b2_body.h>

namespace MoonEngine
{
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define ME_ADD_INTERNAL_CALL(name) mono_add_internal_call("MoonEngine.InternalCalls::" #name, name)

	Entity GetEntity(UUID id)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		Entity e = scene->FindEntityWithUUID(id);
		ME_ASSERT(e, "Entity with given ID not found!");
		return e;
	}

#pragma region Entity

	uint64_t Entity_Instantiate(uint64_t entityId, glm::vec3* position)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		Entity e = scene->FindEntityWithUUID(entityId);
		Entity newE = scene->DuplicateEntity(e);

		newE.GetComponent<TransformComponent>().Position = *position;
		newE.GetComponent<PhysicsBodyComponent>().SetPosition(*position);

		return newE.GetUUID();
	}

	void Entity_Destroy(uint64_t entityId)
	{
		Entity e = GetEntity(entityId);
		e.Destroy();
	}

	static MonoObject* Entity_GetScript(UUID entityId)
	{
		auto instance = ScriptEngine::GetScriptInstance(entityId);
		ME_ASSERT(instance, "Script Instance does not exist!");
		auto obj = instance->GetMonoObject();
		ME_ASSERT(obj, "Mono Object does not exist!");

		return obj;
	}

	bool Entity_HasComponent(uint64_t id, MonoReflectionType* type)
	{
		auto e = GetEntity(id);

		MonoType* monoType = mono_reflection_type_get_type(type);
		ME_ASSERT((s_EntityHasComponentFuncs.find(monoType) != s_EntityHasComponentFuncs.end()), "Has not Mono Type!");
		
		return s_EntityHasComponentFuncs.at(monoType)(e);
	}

#pragma endregion

#pragma region Transform Component

	void Transform_GetPosition(uint64_t id, glm::vec3* position)
	{
		*position = GetEntity(id).GetComponent<TransformComponent>().Position;
	}

	void Transform_SetPosition(uint64_t id, glm::vec3* position)
	{
		GetEntity(id).GetComponent<TransformComponent>().Position = *position;
	}

#pragma endregion

#pragma region Camera

	float Camera_GetSize(uint64_t id)
	{
		return GetEntity(id).GetComponent<CameraComponent>().Size;
	}

	void Camera_SetSize(uint64_t id, float size)
	{
		GetEntity(id).GetComponent<CameraComponent>().Size = size;
	}


#pragma endregion


#pragma region Physics

	static void PhysicsBody_SetBodyType(UUID entityId, PhysicsBodyComponent::BodyType bodyType)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<PhysicsBodyComponent>().Type = bodyType;
		b2Body* body = (b2Body*)entity.GetComponent<PhysicsBodyComponent>().RuntimeBody;
		body->SetType(PhysicsWorld::ConvertBodyType(bodyType));
	}

	static void PhysicsBody_AddForce(UUID entityId, glm::vec2* force, glm::vec2* position)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<PhysicsBodyComponent>().AddForce(*force, *position);
	}

	static void PhysicsBody_AddImpulse(UUID entityId, glm::vec2* force, glm::vec2* position)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<PhysicsBodyComponent>().AddImpulse(*force, *position);
	}

	static void PhysicsBody_AddAngularImpulse(UUID entityId, float impulse)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<PhysicsBodyComponent>().AddAngularImpulse(impulse);
	}

	static void PhysicsBody_AddTorque(UUID entityId, float force)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = scene->FindEntityWithUUID(entityId);
		entity.GetComponent<PhysicsBodyComponent>().AddTorque(force);
	}

#pragma endregion

#pragma region Input 

	static bool Input_GetKey(Keycode keycode)
	{
		return Input::GetKey(keycode);
	}

#pragma endregion

	void ScriptDepot::InitializeCalls()
	{
		//Entity
		ME_ADD_INTERNAL_CALL(Entity_Instantiate);
		ME_ADD_INTERNAL_CALL(Entity_Destroy);
		
		ME_ADD_INTERNAL_CALL(Entity_GetScript);

		ME_ADD_INTERNAL_CALL(Entity_HasComponent);

		//Transform
		ME_ADD_INTERNAL_CALL(Transform_GetPosition);
		ME_ADD_INTERNAL_CALL(Transform_SetPosition);

		//Camera
		ME_ADD_INTERNAL_CALL(Camera_GetSize);
		ME_ADD_INTERNAL_CALL(Camera_SetSize);

		//Physics
		ME_ADD_INTERNAL_CALL(PhysicsBody_SetBodyType);

		ME_ADD_INTERNAL_CALL(PhysicsBody_AddForce);
		ME_ADD_INTERNAL_CALL(PhysicsBody_AddImpulse);
		ME_ADD_INTERNAL_CALL(PhysicsBody_AddAngularImpulse);
		ME_ADD_INTERNAL_CALL(PhysicsBody_AddTorque);

		//Input
		ME_ADD_INTERNAL_CALL(Input_GetKey);
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
		s_EntityHasComponentFuncs.clear();
		RegisterComponent(AllComponents{});
	}
}
