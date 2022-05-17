#pragma once
#include "entt/entt.hpp"
#include "Core.h"

namespace MoonEngine
{
	class Entity;
	class Framebuffer;

	class Scene
	{
	private:
		static Scene* m_ActiveScene;
		entt::registry m_Registry;
		OrderedStack<Entity*> m_Entities;
		friend class Entity;
		friend class EditorLayer;
	public:
		OrthographicCamera* Camera = nullptr;
		Scene();
		~Scene();
		void OnEvent(Event& event);
		void OnPlay();
		void Create(OrthographicCamera* camera);
		void Update();
		void Update(Framebuffer& framebuffer);
		Entity* GetEntityPointer(entt::entity e);
		Entity* CreateEntity();
		void DeleteEntity(int index);
	};
}