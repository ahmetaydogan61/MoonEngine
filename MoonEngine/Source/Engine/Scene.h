#pragma once
#include "Renderer/Renderer.h"

#include <entt.hpp>

class b2World;

namespace MoonEngine
{
	class Entity;
	class Camera;

	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		std::string SceneName = "NewScene";

		Entity CreateEntity();
		Entity DuplicateEntity(Entity& entity);

		void SetRenderer(const Shared<Renderer>& renderer)
		{
			m_Renderer = renderer;
		}

		void StartRuntime();
		void StopRuntime();

		void StartEdit();
		void StopEdit();

		void UpdateRuntime(bool update);
		void UpdateEdit(const Camera* camera);

		static Shared<Scene> CopyScene(Shared<Scene> scene);
	private:
		entt::registry m_Registry;

		Shared<Renderer> m_Renderer;
		Shared<Texture> m_CameraTexture;

		b2World* m_PhysicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;

		friend class EditorLayer;
		friend struct BasicView;
	};
}