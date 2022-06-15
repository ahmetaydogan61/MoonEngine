#pragma once
#include "Scene.h"

namespace MoonEngine
{
	class Entity
	{
	private:
		entt::entity m_ID = entt::null;
		Scene* m_Scene = nullptr;
		friend class Scene;
		friend class HierarchyView;
		friend class Serializer;

		Entity(entt::entity id, Scene* scene)
		{
			m_Scene = scene;
			m_ID = id;
		}
	public:
		Entity() {};

		~Entity() { m_Scene = nullptr; };

		Entity(const Entity& entity)
		{
			m_ID = entity.m_ID;
			m_Scene = entity.m_Scene;
		};
	
		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace<T>(m_ID, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
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
			m_Scene->m_Registry.remove<T>(m_ID);
		}

		void Destroy()
		{
			m_Scene->m_Registry.destroy(m_ID);
		}

		bool operator==(const Entity& other) const { return m_ID == other.m_ID && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
		operator bool() const { return m_ID != entt::null; }
	};
}