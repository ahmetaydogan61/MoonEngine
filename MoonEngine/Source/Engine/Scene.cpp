#include "mpch.h"
#include "Core/Debug.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

namespace MoonEngine
{
	Scene::Scene()
	{
		m_CameraTexture = MakeShared<Texture>("Resource/EditorIcons/Camera.png");
	}

	void Scene::StartPlay()
	{
		ME_LOG("Play Started");
	}

	void Scene::StopPlay()
	{
		ME_LOG("Play Stopped");
	}

	void Scene::StartEdit()
	{
		ME_LOG("Edit Started");
	}

	void Scene::StopEdit()
	{
		ME_LOG("Edit Stopped");
	}

	void Scene::UpdateEdit(const Camera* camera)
	{

	}

	void Scene::UpdatePlay(const glm::vec2& gameViewSize, bool paused)
	{

	}

	template<typename T>
	static void CopyIfExists(Entity copyTo, Entity copyFrom)
	{
		if (copyFrom.HasComponent<T>())
		{
			T component = copyFrom.GetComponent<T>();
			copyTo.AddComponent<T>() = component;
		}
	}

	Entity Scene::CreateEntity()
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<UUIDComponent>();
		entity.AddComponent<IdentityComponent>();
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	Entity Scene::DuplicateEntity(Entity& entity)
	{
		Entity e{ m_Registry.create(), this };
		e.AddComponent<UUIDComponent>();
		CopyIfExists<IdentityComponent>(e, entity);
		CopyIfExists<TransformComponent>(e, entity);
		CopyIfExists<SpriteComponent>(e, entity);
		CopyIfExists<ParticleComponent>(e, entity);
		return e;
	}

	Shared<Scene> Scene::CopyScene(Shared<Scene> scene)
	{
		Shared<Scene> tempScene = MakeShared<Scene>();
		tempScene->SceneName = scene->SceneName;

		scene->m_Registry.each_reverse([&](auto entityID)
		{
			Entity copyFrom{ entityID, scene.get() };
			Entity copyTo = { tempScene->m_Registry.create(), tempScene.get() };
			CopyIfExists<UUIDComponent>(copyTo, copyFrom);
			CopyIfExists<IdentityComponent>(copyTo, copyFrom);
			CopyIfExists<TransformComponent>(copyTo, copyFrom);
			CopyIfExists<SpriteComponent>(copyTo, copyFrom);
			CopyIfExists<CameraComponent>(copyTo, copyFrom);
			CopyIfExists<ParticleComponent>(copyTo, copyFrom);
		});
		return tempScene;
	}
}
