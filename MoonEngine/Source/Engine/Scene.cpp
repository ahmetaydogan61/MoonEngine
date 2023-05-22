#include "mpch.h"
#include "Core/Debug.h"

#include "Core/Time.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "Physics/Collision.h"

#include "Scripting/ScriptEngine.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

namespace MoonEngine
{
	static  std::unordered_map<UUID, Shared<ScriptInstance>> s_ScriptInstances;

	static Scene* s_ActiveScene = nullptr;

	void Scene::SetActiveScene(Scene* scene)
	{
		s_ActiveScene = scene;
	}

	Scene* const Scene::GetActiveScene()
	{
		return s_ActiveScene;
	}

	void Scene::StartRuntime()
	{
		m_PhysicsWorld.BeginWorld();
		m_PhysicsWorld.SetContactListeners(BIND_LISTENER(Scene::OnCollisionBegin), BIND_LISTENER(Scene::OnCollisionEnd));

		auto view = m_Registry.view<const TransformComponent, PhysicsBodyComponent>();
		for (auto [e, transform, pb] : view.each())
		{
			Entity entity{ e, this };
			m_PhysicsWorld.RegisterPhysicsBody(entity, transform, pb);
		}

		auto particleSystemView = m_Registry.view<const TransformComponent, ParticleComponent>();
		for (auto [entity, transformComponent, particle] : particleSystemView.each())
		{
			particle.ParticleSystem.Stop();
			if (particle.ParticleSystem.PlayOnAwake)
				particle.ParticleSystem.Play();
		}


		//+ScriptComponents
		{
			s_ScriptInstances = ScriptEngine::GetScriptInstances();

			auto view = m_Registry.view<ScriptComponent>();
			for (auto [e, script] : view.each())
			{
				Entity entity = { e, this };
				ScriptEngine::AwakeEntity(entity, script.ClassName);
			}
		}
		//-ScriptComponents
	}

	void Scene::StopRuntime()
	{
		m_PhysicsWorld.EndWorld();

		auto particleSystemView = m_Registry.view<const TransformComponent, ParticleComponent>();
		for (auto [entity, transformComponent, particle] : particleSystemView.each())
			particle.ParticleSystem.Stop();

	}

	void Scene::StartEdit()
	{
		ScriptEngine::ClearScriptInstances();

		CreateSciptInstances();

		if (s_ScriptInstances.size() <= 0)
			return;
		
		for (const auto& [uuid, instance] : s_ScriptInstances)
		{
			auto scriptInstance = ScriptEngine::GetScriptInstance(uuid);
			scriptInstance->SetInstanceFields(instance->GetInstanceFields());
		}

		s_ScriptInstances.clear();
	}

	void Scene::StopEdit()
	{
	}

	void Scene::UpdateEdit(const Camera* camera)
	{

	}

	void Scene::CreateSciptInstances()
	{
		auto view = m_Registry.view<ScriptComponent>();
		for (auto [e, script] : view.each())
		{
			Entity entity = { e, this };
			ScriptEngine::CreateEntityInstance(entity, script.ClassName);
		}
	}

	void Scene::UpdateRuntime(bool update)
	{
		float dt = Time::DeltaTime();

		if (update)
		{
			//Script Components
			{
				auto view = m_Registry.view<ScriptComponent>();
				for (auto [e, script] : view.each())
				{
					Entity entity = { e, this };
					ScriptEngine::UpdateEntity(entity, script.ClassName, dt);
				}
			}

			//PhysicsWorld
			{
				auto view = m_Registry.view<TransformComponent, const PhysicsBodyComponent>();

				m_PhysicsWorld.StepWorld(dt, [&]
				{
					for (auto [e, transform, physicsBody] : view.each())
						m_PhysicsWorld.ResetPhysicsBodies(Entity{ e, this }, transform, physicsBody);
				});

				for (auto [e, transform, physicsBody] : view.each())
					m_PhysicsWorld.UpdatePhysicsBodies(Entity{ e, this }, transform, physicsBody);
			}

			//Particle System
			{
				auto view = m_Registry.view<const TransformComponent, ParticleComponent>();
				for (auto [entity, transformComponent, particle] : view.each())
				{
					particle.ParticleSystem.UpdateEmitter(dt, particle.Particle, transformComponent.Position);
					particle.ParticleSystem.UpdateParticles(dt);
				}
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
		return CreateEntity(UUID());
	}

	Entity Scene::CreateEntity(UUID uuid)
	{
		entt::entity entt = m_Registry.create();
		Entity entity = { entt, this };

		entity.AddComponent<UUIDComponent>().ID = uuid;
		m_UUIDRegistry[uuid] = entt;

		entity.AddComponent<IdentityComponent>();
		entity.AddComponent<TransformComponent>();
		return entity;
	}

	Entity Scene::DuplicateEntity(Entity from)
	{
		entt::entity entt = m_Registry.create();
		Entity to = { entt, this };

		UUID uuid = to.AddComponent<UUIDComponent>().ID;
		m_UUIDRegistry[uuid] = entt;

		CopyIfExists<IdentityComponent>(to, from);
		CopyIfExists<TransformComponent>(to, from);
		CopyIfExists<SpriteComponent>(to, from);
		CopyIfExists<CameraComponent>(to, from);
		CopyIfExists<ParticleComponent>(to, from);
		CopyIfExists<PhysicsBodyComponent>(to, from);
		CopyIfExists<ScriptComponent>(to, from);

		return to;
	}

	void Scene::DestroyEntity(Entity e)
	{
		m_UUIDRegistry.erase(e.GetUUID());

		RemoveIfExists<ScriptComponent>(e);
		RemoveIfExists<PhysicsBodyComponent>(e);
		RemoveIfExists<ParticleComponent>(e);
		RemoveIfExists<CameraComponent>(e);
		RemoveIfExists<SpriteComponent>(e);
		RemoveIfExists<IdentityComponent>(e);
		RemoveIfExists<TransformComponent>(e);
		RemoveIfExists<UUIDComponent>(e);

		m_Registry.destroy(e.m_ID);
	}

	Shared<Scene> Scene::CopyScene(Shared<Scene> scene)
	{
		Shared<Scene> tempScene = MakeShared<Scene>();
		tempScene->SceneName = scene->SceneName;
		tempScene->m_UUIDRegistry = scene->m_UUIDRegistry;

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
			CopyIfExists<ScriptComponent>(copyTo, copyFrom);
		});
		return tempScene;
	}

	Entity Scene::FindEntityWithUUID(UUID uuid)
	{
		if (m_UUIDRegistry.find(uuid) != m_UUIDRegistry.end())
			return { m_UUIDRegistry.at(uuid), this };

		ME_LOG("Entity with this UUID not found!");
		return {};
	}

	Entity Scene::FindEntityWithName(std::string_view name)
	{
		auto view = m_Registry.view<IdentityComponent>();
		for (auto [e, idendity] : view.each())
			if (name == idendity.Name)
				return Entity{ e, this };
		return {};
	}

	void Scene::OnCollisionBegin(void* collisionA, void* collisionB)
	{
		Collision* cA = (Collision*)collisionA;
		Collision* cB = (Collision*)collisionB;

		if (!cB || !cA)
			return;

		cA->PhysicsBody->OnCollisionEnter.Invoke(cB);
		cB->PhysicsBody->OnCollisionEnter.Invoke(cA);
	}

	void Scene::OnCollisionEnd(void* collisionA, void* collisionB)
	{
		Collision* cA = (Collision*)collisionA;
		Collision* cB = (Collision*)collisionB;

		if (!cB || !cA)
			return;

		cA->PhysicsBody->OnCollisionExit.Invoke(cB);
		cB->PhysicsBody->OnCollisionExit.Invoke(cA);
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
	void Scene::OnAddComponent(Entity entity, ScriptComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, ScriptComponent& component)
	{
		ScriptEngine::DestroyEntity(entity);
	}

	template<>
	void Scene::OnAddComponent(Entity entity, ParticleComponent& component) {}

	template<>
	void Scene::OnRemoveComponent(Entity entity, ParticleComponent& component) {}

	template<>
	void Scene::OnAddComponent(Entity entity, PhysicsBodyComponent& component)
	{
		if (m_PhysicsWorld.WorldExists())
			m_PhysicsWorld.RegisterPhysicsBody(entity, entity.GetComponent<TransformComponent>(), component, true);
	}

	template<>
	void Scene::OnRemoveComponent(Entity entity, PhysicsBodyComponent& component)
	{
		if (m_PhysicsWorld.WorldExists())
			m_PhysicsWorld.UnregisterPhysicsBody(component, true);
	}
}
