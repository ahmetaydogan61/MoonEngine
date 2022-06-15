#pragma once
#include "entt/entt.hpp"
#include "Core.h"
#include "Utils/EditorCamera.h"

namespace MoonEngine
{
	class Entity;
	class Framebuffer;
	class ParticleSystem;

	class Scene
	{
	private:
		entt::registry m_Registry;
		float m_ViewportHeight;
		float m_ViewportWidth;

		friend class Entity;
		friend class HierarchyView;
		friend class Serializer;
	public:
		std::string SceneName = "Untitled";
		Scene() = default;
		~Scene() = default;
		static Ref<Scene> CopyScene(Ref<Scene> scene);

		void OnPlay();
		void OnStop();
		void UpdateRuntime();
		void UpdateEditor(const EditorCamera* camera, Entity& entity);
		void ResizeViewport(float width, float height);
		
		Entity CreateEntity();
		Entity CreateCameraEntity();
		Entity DuplicateEntity(Entity& entity);
	};
}