#pragma once
#include "Engine/Systems/ParticleSystem.h"
#include "Physics/Physics.h"

#include "Engine/UUID.h"

#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/TextureSheet.h"

namespace MoonEngine
{
	struct UUIDComponent
	{
		UUID ID;
		UUIDComponent() = default;
		UUIDComponent(const UUIDComponent&) = default;
	};

	struct IdentityComponent
	{
		std::string Name = "Entity";
		REFLECT(("Name", Name))
	};

	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);

		REFLECT(("Position", Position)("Rotation", Rotation)("Scale", Scale))
	};

	struct SpriteComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		glm::vec2 Tiling = glm::vec2(1.0f);
		int Layer;
		glm::vec2 SpriteCoords;
		glm::vec2 SpriteSize;

		void SetTexture(Shared<Texture> texture) 
		{ 
			m_Texture = texture;

			if (m_TextureSheet)
				GenerateSpriteSheet();
		}

		void DeleteTexture() { m_Texture = nullptr; if (m_TextureSheet) DeleteSpriteSheet(); }
		const Shared<Texture>& GetTexture() const { return m_Texture; }

		void GenerateSpriteSheet()
		{
			if (!m_Texture)
				return;

			m_TextureSheet = MakeShared<TextureSheet>();
			m_TextureSheet->Create(m_Texture);
			m_TextureSheet->Resize(SpriteSize);
			m_TextureSheet->SetCoordinate(SpriteCoords);
			m_HasSpriteSheet = true;
		}

		void DeleteSpriteSheet() { m_TextureSheet = nullptr; m_HasSpriteSheet = false; }
		const Shared<TextureSheet>& GetTextureSheet() const { return m_TextureSheet; }
		bool HasSpriteSheet() const { return m_HasSpriteSheet; }

		REFLECT
		(
			("Color", Color)("Tiling", Tiling)("Texture", m_Texture)("Layer", Layer)
			("SpriteCoords", SpriteCoords)("SpriteSize", SpriteSize)("HasSpriteSheet", m_HasSpriteSheet)
		)

	private:
		Shared<Texture> m_Texture = nullptr;
		Shared<TextureSheet> m_TextureSheet;
		bool m_HasSpriteSheet = false;
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

			Resize(m_AspectRatio);
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

		REFLECT(("IsMain", IsMain)("Size", Size))
	};

	struct ScriptComponent
	{
		std::string ClassName;
		bool HasValidClass = false;
		uint64_t EntityHandle;

		REFLECT(("ClassName", ClassName))
	};

	struct ParticleComponent
	{
		ParticleBody Particle;
		ParticleSystem ParticleSystem;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents = ComponentGroup
		<TransformComponent, SpriteComponent, CameraComponent, ScriptComponent, PhysicsBodyComponent, ParticleComponent>;
}