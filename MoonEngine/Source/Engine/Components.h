#pragma once

#include "Renderer/Camera.h"
#include "Renderer/Texture.h"

#include <uuid/uuid_v4.h>

namespace MoonEngine
{
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
		std::string Name = "Entity";
	};

	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
	};

	struct SpriteComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec2 Tiling = glm::vec2(1.0f);
		Shared<Texture> Texture = nullptr;
	};

	struct CameraComponent
	{
	private:
		float m_AspectRatio = 1.0f;
		float m_NearClip = -1.0f;
		float m_FarClip = 1.0f;

		glm::mat4 m_Projection = glm::mat4(1.0f);
	public:
		CameraComponent() { Resize(16.0f / 9.0f); }
		~CameraComponent() = default;
		CameraComponent(const CameraComponent& camera) = default;

		bool IsMain = true;
		float Size = 1.0f;

		void Zoom(float amount)
		{
			Size += amount;
			if (Size < 0.1f)
				Size = 0.1f;

			float left = -m_AspectRatio * Size;
			float right = m_AspectRatio * Size;
			float bottom = -Size;
			float top = Size;

			m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
		}

		void Resize(float aspectRatio)
		{
			m_AspectRatio = aspectRatio;
			float left = -aspectRatio * Size;
			float right = aspectRatio * Size;
			float bottom = -Size;
			float top = Size;
			m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
		}

		const glm::mat4& GetProjection() const { return m_Projection; }
	};
}