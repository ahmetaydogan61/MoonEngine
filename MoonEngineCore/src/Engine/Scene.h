#pragma once
#include "Utils/EditorCamera.h"

#include "entt/entt.hpp"

namespace MoonEngine
{
	class Entity;
	class ParticleSystem;

	class Scene
	{
	private:
		entt::registry m_Registry;
		float m_ViewportHeight = 0;
		float m_ViewportWidth = 0;

		friend class Entity;
		friend class HierarchyView;
		friend class Serializer;
		friend class ResourceManager;
	public:
		std::string SceneName = "Untitled";
		Scene() = default;
		~Scene() = default;
		static Ref<Scene> CopyScene(Ref<Scene> scene);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnEditorStart();
		void OnEditorStop();
		void UpdateRuntime(bool isPlaying);
		void UpdateEditor(const EditorCamera* camera, Entity& entity);
		void ResizeViewport(float width, float height);
		
		Entity CreateEntity();
		Entity CreateCameraEntity();
		Entity DuplicateEntity(Entity& entity);
		template<typename T>
		void CopyComponent(Entity& copyTo, Entity& copyFrom);
	};
}