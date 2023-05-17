#pragma once
#include "Engine/Scene.h"

namespace MoonEngine
{
	class Entity
	{
	public:
		Entity() {};
		~Entity() { m_Scene = nullptr; };

		Entity(const Entity& entity)
		{
			m_ID = entity.m_ID;
			m_Scene = entity.m_Scene;
		};

		Entity(entt::entity id, Scene* scene)
		{
			m_Scene = scene;
			m_ID = id;
		}

		bool operator==(const Entity& other) const { return m_ID == other.m_ID && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
		operator bool() const { return m_ID != entt::null; }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace<T>(m_ID, std::forward<Args>(args)...);
			m_Scene->OnAddComponent<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			ME_ASSERT(HasComponent<T>(), "Entity does not have this component!");
			return m_Scene->m_Registry.get<T>(m_ID);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.any_of<T>(m_ID);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Scene->OnRemoveComponent<T>(*this, GetComponent<T>());
			m_Scene->m_Registry.remove<T>(m_ID);
		}

		void Destroy()
		{
			m_Scene->DestroyEntity(*this);
		}


		const std::string GetUUID() { return GetComponent<UUIDComponent>().ID.str(); }
		const std::string Name() { return GetComponent<IdentityComponent>().Name; }

	private:
		entt::entity m_ID = entt::null;
		Scene* m_Scene = nullptr;
		
		friend class Scene;
		friend class PhysicsWorld;
		friend class ScriptInstance;
	};
}