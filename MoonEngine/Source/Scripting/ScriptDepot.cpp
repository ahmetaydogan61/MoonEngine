#include "mpch.h"
#include "Scripting/ScriptDepot.h"
#include "Scripting/ScriptEngine.h"

#include "Core/Input.h"

#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "mono/metadata/object.h"

namespace MoonEngine
{
#define ME_ADD_INTERNAL_CALL(name) mono_add_internal_call("MoonEngine.InternalCalls::" #name, name)

	static void Entity_SetPosition(int entityID, glm::vec3* refPosition)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = Entity{ (entt::entity)entityID, scene };
		entity.GetComponent<TransformComponent>().Position = *refPosition;
	}

	static void Entity_GetPosition(int entityID, glm::vec3* outPosition)
	{
		Scene* scene = ScriptEngine::GetRuntimeScene();
		auto entity = Entity{ (entt::entity)entityID, scene };
		*outPosition = entity.GetComponent<TransformComponent>().Position;
	}

	static bool Input_GetKey(Keycode keycode)
	{
		return Input::GetKey(keycode);
	}

	void ScriptDepot::InitializeScripts()
	{
		ME_ADD_INTERNAL_CALL(Entity_SetPosition);
		ME_ADD_INTERNAL_CALL(Entity_GetPosition);

		ME_ADD_INTERNAL_CALL(Input_GetKey);
	}
}
