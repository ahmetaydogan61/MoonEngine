#pragma once
#include "Physics/PhysicsWorld.h"

#include "Renderer/Renderer.h"

#include <entt.hpp>

namespace MoonEngine
{
	class Entity;
	class Camera;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		std::string SceneName = "New Scene";

		Entity CreateEntity();
		Entity CreateEntity(UUID uuid);
		void DestroyEntity(Entity e);
		Entity DuplicateEntity(Entity entity);
		  
		void StartRuntime();
		void StopRuntime();

		void StartEdit();
		void StopEdit();

		void UpdateRuntime(bool update);
		void UpdateEdit(const Camera* camera);

		void CreateSciptInstances();

		Entity FindEntityWithUUID(UUID uuid);
		Entity FindEntityWithName(std::string_view name);

		static Shared<Scene> CopyScene(Shared<Scene> scene);
		static Scene* const GetActiveScene();
	private:
		entt::registry m_Registry;
		std::unordered_map<UUID, entt::entity> m_UUIDRegistry;

		PhysicsWorld m_PhysicsWorld;
		void OnCollisionBegin(void*, void*);
		void OnCollisionEnd(void*, void*);

		template<typename T>
		void OnAddComponent(Entity entity, T& component);
		template<typename T>
		void OnRemoveComponent(Entity entity, T& component);

		static void SetActiveScene(Scene* scene);

		friend class Entity;
		friend class SceneSerializer;
		friend class PhysicsWorld;

		friend struct BasicView;
		friend class EditorLayer;
	};
}