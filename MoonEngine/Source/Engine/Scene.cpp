#include "mpch.h"
#include "Core/Debug.h"

#include "Core/Time.h"

#include "Engine/Components.h"
#include "Engine/Entity.h"
#include "Engine/Scene.h"

#include "Renderer/Camera.h"
#include "Renderer/Renderer.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace MoonEngine
{
	Scene::Scene()
	{
		m_CameraTexture = MakeShared<Texture>("Resource/EditorIcons/Camera.png");
	}

	static b2BodyType ConvertBodyType(RigidbodyComponent::BodyType type)
	{
		switch (type)
		{
			case MoonEngine::RigidbodyComponent::BodyType::Static: return b2_staticBody;
			case MoonEngine::RigidbodyComponent::BodyType::Dynamic: return b2_dynamicBody;
			case MoonEngine::RigidbodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		ME_ASSERT(false, "Invalid body type!");
		return b2_staticBody;
	}

	void Scene::StartRuntime()
	{
		ME_LOG("Runtime Started");

		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_Registry.view<RigidbodyComponent>();
		for (auto e : view)
		{
			ME_LOG("Rigidbody Updated");
			Entity entity{ e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb = entity.GetComponent<RigidbodyComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = ConvertBodyType(rb.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb.FreezeRotation);

			rb.RuntimeBody = body;

			if (entity.HasComponent<BoxColliderComponent>())
			{
				auto& boxColl = entity.GetComponent<BoxColliderComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(transform.Scale.x * boxColl.Size.x, transform.Scale.y * boxColl.Size.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = boxColl.Density;
				fixtureDef.friction = boxColl.Friction;
				fixtureDef.restitution = boxColl.Restitution;
				fixtureDef.restitutionThreshold = boxColl.RestitutionThreshold;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::StopRuntime()
	{
		ME_LOG("Runtime Stopped");

		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
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
				const int32_t velocityIterations = 6;
				const int32_t positionIterations = 2;
				m_PhysicsWorld->Step(Time::DeltaTime(), velocityIterations, positionIterations);

				auto view = m_Registry.view<RigidbodyComponent>();
				for (auto e : view)
				{
					Entity entity{ e, this };
					auto& transform = entity.GetComponent<TransformComponent>();
					auto& rigidbody = entity.GetComponent<RigidbodyComponent>();

					auto* body = (b2Body*)rigidbody.RuntimeBody;
					const auto& position = body->GetPosition();
					transform.Position.x = position.x;
					transform.Position.y = position.y;

					transform.Rotation.z = body->GetAngle();

					ME_LOG("Rigidbodies are being updated");
				}
			}
		}

		//SpriteRenderer
		{
			auto view = m_Registry.view<const TransformComponent, const SpriteComponent>();
			for (auto [entity, transform, sprite] : view.each())
			{
				Renderer::DrawEntity(transform, sprite, (int)entity);
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
		CopyIfExists<RigidbodyComponent>(e, entity);
		CopyIfExists<BoxColliderComponent>(e, entity);
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
			CopyIfExists<RigidbodyComponent>(copyTo, copyFrom);
			CopyIfExists<BoxColliderComponent>(copyTo, copyFrom);
		});
		return tempScene;
	}
}
