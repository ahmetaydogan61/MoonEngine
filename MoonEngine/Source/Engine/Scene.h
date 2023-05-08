#pragma once
#include "Renderer/Renderer.h"

#include <entt.hpp>

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

		void StartPlay();
		void StopPlay();

		void StartEdit();
		void StopEdit();

		void UpdatePlay(const glm::vec2& gameViewSize, bool paused);
		void UpdateEdit(const Camera* camera);

		static Shared<Scene> CopyScene(Shared<Scene> scene);
	private:
		entt::registry m_Registry;

		Shared<Renderer> m_Renderer;
		Shared<Texture> m_CameraTexture;

		friend class Entity;
		friend class SceneSerializer;

		friend class EditorLayer;
		friend struct BasicView;
	};
}