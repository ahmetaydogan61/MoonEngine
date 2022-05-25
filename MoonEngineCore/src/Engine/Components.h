#pragma once
#include "ScriptableEntity.h"
#include "SceneCamera.h"

namespace MoonEngine
{
	class Scene;

	struct IdentityComponent
	{
		std::string Name;

		IdentityComponent() = default;
		IdentityComponent(const IdentityComponent&) = default;
		IdentityComponent(const std::string& name)
			: Name(name)
		{}
	};

	struct TransformComponent
	{
		glm::vec3 position{ 0.0f };
		glm::vec3 size{ 1.0f };

		TransformComponent() = default;
		TransformComponent(glm::vec3& position)
			:position(position)
		{}
		TransformComponent(const TransformComponent& transform) = default;
	};

	struct SpriteComponent
	{
		glm::vec4 color{ 1.0f };

		SpriteComponent() = default;
		SpriteComponent(glm::vec4& color)
			:color(color)
		{}
		SpriteComponent(const SpriteComponent& sprite) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool isMain = true;
		float distance = 5.0f;

		CameraComponent()
		{
			Camera.Resize(1.0f, 1.0f, distance);
			isMain = true;
		};
		~CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct Script
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(Script*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](Script* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};
}