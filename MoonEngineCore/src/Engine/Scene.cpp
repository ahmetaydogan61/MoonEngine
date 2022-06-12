#include "mpch.h"
#include "Scene.h"
#include "Core/Window.h"
#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"
#include "Renderer/Frambuffer.h"
#include "glad/glad.h"
#include "Systems/ParticleSystem.h"

namespace MoonEngine
{
	Scene* Scene::m_ActiveScene;

	Entity entity;

	Scene::Scene()
	{
		m_ActiveScene = this;
		DebugSys("Scene Created");
	}

	void Scene::OnPlay()
	{}

	void Scene::OnReset()
	{}

	void Scene::UpdateEditor(const EditorCamera* camera, Entity& entity)
	{
		if (camera)
		{
			Renderer::Begin(camera->GetViewProjection());
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);
				Renderer::DrawQuad(transform.Position, transform.Rotation, transform.Size, sprite.Color, sprite.Texture);
			}

			Renderer::End();

			if (entity)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glLineWidth(5.0f);
				TransformComponent& entityTransform = entity.GetComponent<TransformComponent>();
				Renderer::DrawQuad(entityTransform.Position, entityTransform.Rotation, entityTransform.Size, glm::vec4(0.0f, 0.58f, 1.0f, 1.0f));
				Renderer::End();
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}

	void Scene::UpdateRuntime()
	{
		m_Registry.view<Script>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity(entity);
				nsc.Instance->Awake();
			}
			nsc.Instance->Update();
		});

		Camera* sceneCamera = nullptr;
		glm::vec3 cameraPosition;

		auto cameras = m_Registry.view<TransformComponent, CameraComponent>();
		for (auto entity : cameras)
		{
			auto [transform, camera] = cameras.get<TransformComponent, CameraComponent>(entity);
			if (camera.IsMain)
			{
				sceneCamera = &camera.Camera;
				cameraPosition = transform.Position;
				break;
			}
		}

		glm::mat4 viewProjection;


		if (sceneCamera)
		{
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, 0.0f));
			glm::mat4 m_View = glm::inverse(transform);
			viewProjection = sceneCamera->GetProjection() * m_View;
		}
		Renderer::Begin(viewProjection);
		float deltaTime = Time::DeltaTime();

		{//+Particle System
			auto particleGroup = m_Registry.group<ParticleComponent>(entt::get<TransformComponent>);
			for (auto entity : particleGroup)
			{
				auto [particle, transform] = particleGroup.get<ParticleComponent, TransformComponent>(entity);
				if (particle.Play)
				{
					if (particle.m_DurationElapsed == 0 && particle.BurstMode)
						particle.Spawn(particle, transform.Position);
					else if (!particle.BurstMode)
						particle.Spawn(particle, transform.Position);

					particle.m_DurationElapsed += deltaTime;
					if (particle.m_DurationElapsed >= particle.Duration)
					{
						particle.m_DurationElapsed = 0.0f;
						particle.Play = particle.AutoPlay;
					}
				}
				else
					particle.m_DurationElapsed = 0.0f;

				particle.Update();
			}
		}//-Particle System

		{//+Sprite Rendering
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteComponent>);
			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteComponent>(entity);
				Renderer::DrawQuad(transform.Position, transform.Rotation, transform.Size, sprite.Color, sprite.Texture);
			}
		}//-Sprite Rendering
		Renderer::End();
	}

	void Scene::ResizeViewport(float width, float height)
	{
		auto camera = m_Registry.view<CameraComponent>();
		for (auto entity : camera)
		{
			CameraComponent& component = camera.get<CameraComponent>(entity);
			component.Camera.Resize(width, height, component.Distance);
		}
	}

	template<typename T>
	void CopyIfExists(Entity& copyTo, Entity& copyFrom)
	{
		if (copyFrom.HasComponent<T>())
		{
			T& component = copyTo.AddComponent<T>(); 
			component = copyFrom.GetComponent<T>();
		}
	}

	Entity Scene::CopyEntity(Entity& entity)
	{
		Entity e{ m_Registry.create() };
		e.AddComponent<UUIDComponent>();
		CopyIfExists<IdentityComponent>(e, entity);
		CopyIfExists<TransformComponent>(e, entity);
		CopyIfExists<SpriteComponent>(e, entity);
		CopyIfExists<CameraComponent>(e, entity);
		CopyIfExists<ParticleComponent>(e, entity);
		return e;
	}

	Entity Scene::CreateEntity()
	{
		Entity e{ m_Registry.create() };
		e.AddComponent<UUIDComponent>();
		e.AddComponent<IdentityComponent>();
		e.GetComponent<IdentityComponent>().Name = "Entity";
		e.AddComponent<TransformComponent>();
		e.AddComponent<SpriteComponent>();
		return e;
	}

	Entity Scene::CreateCameraEntity()
	{
		Entity e{ m_Registry.create() };
		e.AddComponent<UUIDComponent>();
		e.AddComponent<IdentityComponent>();
		e.GetComponent<IdentityComponent>().Name = "Camera";
		e.AddComponent<TransformComponent>();
		e.AddComponent<CameraComponent>();
		return e;
	}

	Scene::~Scene()
	{
		DebugSys("Scene Destroyed");
	}
}
