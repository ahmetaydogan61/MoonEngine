#pragma once
#include "ScriptableEntity.h"
#include "SceneCamera.h"
#include "Renderer/Texture.h"

#include "endianness.h"
#include "uuid_v4.h"

#include "Systems/ParticleSystem.h"

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
		glm::vec3 Rotation{ 0.0f };
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
		bool IsMain = true;
		float Distance = 5.0f;

		CameraComponent()
		{
			Camera.Resize(1.0f, 1.0f, Distance);
			IsMain = true;
		};
		~CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;
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

	struct ParticleComponent
	{
	private:
		ParticleSystem m_ParticleSystem;
		float m_DurationElapsed = 0.0f;
		friend class Scene;
	public:
		ParticleComponent()
		{
			m_ParticleSystem.ResizePool(MaxParticles);
		}
		ParticleComponent(const ParticleComponent& particle) = default;
		~ParticleComponent() = default;

		//General
		int Rate = 5, MaxParticles = 5000;
		bool BurstMode = false, AutoPlay = true, Play = true;
		float Duration = 0.75f, Lifetime = 1.5f;

		//Position
		glm::vec3 Direction{}, DirectionVelocity = { 1.0f, 1.0f, 0.0f }, ShapeSize{};

		//Size
		glm::vec3 SizeStart = { 0.5f, 0.5f, 0.0f }, SizeEnd = { 0.0f, 0.0f, 0.0f };
		float SizeChangeSpeed = 1.0f;

		//Color
		Ref<Texture> Texture = nullptr;
		glm::vec4 ColorStart = { 1.0f, 0.0f, 0.0f, 1.0f }, ColorEnd = { 0.0f, 0.4f, 1.0f, 1.0f };
		float ColorChangeSpeed = 1.0f;

		//Physics
		bool UsePhysics = false;
		float Gravity = -9.8f, Mass = 1.0f;

		int PoolSize()
		{
			return m_ParticleSystem.PoolSize();
		}

		void Resize()
		{
			m_ParticleSystem.ResizePool(MaxParticles);
		}

		void Spawn(const ParticleComponent& component, const glm::vec3 position)
		{
			for (int i = 0; i < Rate; i++)
				m_ParticleSystem.Spawn(component, position);
		}

		void Update()
		{
			m_ParticleSystem.Update();
		}
	};
}