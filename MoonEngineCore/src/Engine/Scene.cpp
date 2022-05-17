#include "mpch.h"
#include "Scene.h"
#include "Core/Window.h"
#include "Utils/CameraControllerOrthographic.h"
#include "Entity.h"
#include "Components.h"
#include "Renderer/Renderer.h"
#include "Renderer/Frambuffer.h"
#include "glad/glad.h"

namespace MoonEngine
{
	Scene* Scene::m_ActiveScene;

	Scene::Scene()
	{
		m_ActiveScene = this;
		DebugSys("Scene Created");
	}

	void Scene::OnEvent(Event& e)
	{
		Camera->OnEvent(e);
	}

	void Scene::Create(OrthographicCamera* camera)
	{
		Camera = camera;
	}

	void Scene::OnPlay()
	{
	}

	void Scene::Update()
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

		Renderer::Clear();

		auto group = m_Registry.group<Transform>(entt::get<Sprite>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<Transform, Sprite>(entity);
			Renderer::DrawQuad(transform.position, transform.size, sprite.color);
		}

		if (Camera)
			Renderer::Render(*Camera);
	}

	void Scene::Update(Framebuffer& framebuffer)
	{
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
		}

		auto group = m_Registry.group<Transform>(entt::get<Sprite>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<Transform, Sprite>(entity);
			Renderer::DrawQuad(transform.position, transform.size, sprite.color);
		}

		framebuffer.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Camera)
			Renderer::Render(*Camera);

		framebuffer.Unbind();
		glClear(GL_COLOR_BUFFER_BIT);
	}

	Entity* Scene::GetEntityPointer(entt::entity e)
	{
		return new Entity(e);
	}

	Entity* Scene::CreateEntity()
	{
		Entity* e = new Entity(m_Registry.create());
		e->AddComponent<Identity>();
		e->GetComponent<Identity>().Name = "Entity";
		e->AddComponent<Transform>();
		e->AddComponent<Sprite>();
		m_Entities.Push(e);
		return e;
	}

	void Scene::DeleteEntity(int index)
	{
		m_Entities.Get(index)->Destroy();
		m_Entities.Pop(index);
	}

	Scene::~Scene()
	{
		delete Camera;
		DebugSys("Scene Destroyed");
	}
}
