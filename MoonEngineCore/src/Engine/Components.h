#pragma once
#include "ScriptableEntity.h"

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
	};

	struct Sprite
	{
		glm::vec3 color{ 1.0f };

		Sprite() = default;
		Sprite(glm::vec3 color)
			:color(color)
		{}
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