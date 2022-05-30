#pragma once
#include "ScriptableEntity.h"
#include "SceneCamera.h"
#include "Renderer/Texture.h"

#include "endianness.h"
#include "uuid_v4.h"

namespace MoonEngine
{
	class Scene;

	struct UUIDComponent
	{
		UUIDv4::UUID ID;
		UUIDComponent()
		{
			UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
			ID = uuidGenerator.getUUID();
		}
		UUIDComponent(const UUIDComponent&) = default;
	};

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
		glm::vec3 Position{ 0.0f };
		glm::vec3 Size{ 1.0f };

		TransformComponent() = default;
		TransformComponent(glm::vec3& position)
			:Position(position)
		{}
		TransformComponent(const TransformComponent& transform) = default;
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f };
		Ref<Texture> Texture;

		SpriteComponent() = default;
		SpriteComponent(glm::vec4& color)
			:Color(color)
		{}
		SpriteComponent(const SpriteComponent& sprite) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool isMain = true;
		float Distance = 5.0f;

		CameraComponent()
		{
			Camera.Resize(1.0f, 1.0f, Distance);
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