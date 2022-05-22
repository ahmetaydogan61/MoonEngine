#pragma once
#include "ScriptableEntity.h"
#include "SceneCamera.h"

namespace MoonEngine
{
	class Scene;

	struct Identity
	{
		std::string Name;

		Identity() = default;
		Identity(const Identity&) = default;
		Identity(const std::string& name)
			: Name(name)
		{}
	};

	struct Transform
	{
		glm::vec2 position{ 0.0f };
		glm::vec2 size{ 1.0f };

		Transform() = default;
		Transform(glm::vec2 position)
			:position(position)
		{}
		Transform(const Transform& transform) = default;
	};

	struct Sprite
	{
		glm::vec3 color{ 1.0f };

		Sprite() = default;
		Sprite(glm::vec3 color)
			:color(color)
		{}
		Sprite(const Sprite& sprite) = default;
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