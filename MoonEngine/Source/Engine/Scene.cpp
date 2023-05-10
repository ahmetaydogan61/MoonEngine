#include "mpch.h"
#include "Core/Debug.h"

#include "Core/Time.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

namespace MoonEngine
{
	Scene::Scene()
	{
	}

	void Scene::StartRuntime()
	{
		ME_LOG("Runtime Started");

		m_PhysicsWorld.BeginWorld();

		auto view = m_Registry.view<const TransformComponent, PhysicsBodyComponent>();
		for (auto [e, transform, pb] : view.each())
		{
			Entity entity{ e, this };
			m_PhysicsWorld.RegisterPhysicsBody(entity, transform, pb);
		}
	}

	void Scene::StopRuntime()
	{
		ME_LOG("Runtime Stopped");
		m_PhysicsWorld.EndWorld();
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

	void Scene::UpdateRuntime(bool update)
	{
		if (update)
		{
			//PhysicsWorld
			{
				m_PhysicsWorld.StepWorld(Time::DeltaTime());

				auto view = m_Registry.view<TransformComponent, const PhysicsBodyComponent>();
				for (auto [e, transform, physicsBody] : view.each())
					m_PhysicsWorld.UpdatePhysicsBodies(Entity{ e, this }, transform, physicsBody);
			}
		}
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

	template<typename T>
	static void RemoveIfExists(Entity removeFrom)
	{
		if (removeFrom.HasComponent<T>())
		{
			T component = removeFrom.GetComponent<T>();
			removeFrom.RemoveComponent<T>();
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

	void Scene::DestroyEntity(Entity e)
	{
		RemoveIfExists<UUIDComponent>(e);
		RemoveIfExists<IdentityComponent>(e);
		RemoveIfExists<TransformComponent>(e);
		RemoveIfExists<SpriteComponent>(e);
		RemoveIfExists<ParticleComponent>(e);
		RemoveIfExists<PhysicsBodyComponent>(e);
		m_Registry.destroy(e.m_ID);
	}

	Entity Scene::DuplicateEntity(Entity& entity)
	{
		Entity e{ m_Registry.create(), this };
		e.AddComponent<UUIDComponent>();
		CopyIfExists<IdentityComponent>(e, entity);
		CopyIfExists<TransformComponent>(e, entity);
		CopyIfExists<SpriteComponent>(e, entity);
		CopyIfExists<ParticleComponent>(e, entity);
		CopyIfExists<PhysicsBodyComponent>(e, entity);
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
			CopyIfExists<PhysicsBodyComponent>(copyTo, copyFrom);
		});
		return tempScene;
	}

	template<>
	void Scene::OnAddComponent(Entity entity, UUIDComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, UUIDComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, IdentityComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, IdentityComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, TransformComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, TransformComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, SpriteComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, SpriteComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, CameraComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, CameraComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, ParticleComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, ParticleComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, PhysicsBodyComponent& component) 
	{
	
	}

	template<>
	void Scene::OnRemoveComponent(Entity entity, PhysicsBodyComponent& component)
	{
		m_PhysicsWorld.UnregisterPhysicsBody(entity, component);
	}
}
