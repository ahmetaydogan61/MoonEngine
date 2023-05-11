#pragma once
#include "Renderer/Renderer.h"

#include <entt.hpp>

#include "Physics/PhysicsWorld.h"

namespace MoonEngine
{
	class Entity;
	class Camera;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		std::string SceneName = "New Scene";

		Entity CreateEntity();
		void DestroyEntity(Entity e);
		Entity DuplicateEntity(Entity& entity);
		  
		void StartRuntime();
		void StopRuntime();

		void StartEdit();
		void StopEdit();

		void UpdateRuntime(bool update);
		void UpdateEdit(const Camera* camera);

		static Shared<Scene> CopyScene(Shared<Scene> scene);

		Entity FindEntityWithUUID(const std::string& uuid);
	private:
		entt::registry m_Registry;
		std::unordered_map<std::string, entt::entity> m_UUIDRegistry;

		PhysicsWorld m_PhysicsWorld;
		void OnCollisionBegin(void*, void*);
		void OnCollisionEnd(void*, void*);

		template<typename T>
		void OnAddComponent(Entity entity, T& component);
		template<typename T>
		void OnRemoveComponent(Entity entity, T& component);

		friend class Entity;
		friend class SceneSerializer;
		friend class PhysicsWorld;

		friend struct BasicView;
		friend class EditorLayer;
	};
}