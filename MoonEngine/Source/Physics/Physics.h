#pragma once

namespace MoonEngine
{
	
	struct RigidbodyComponent
	{
		enum class BodyType { Static, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FreezeRotation = false;

		void* RuntimeBody = nullptr;

		REFLECT(("Type", Type)("FreezeRotation", FreezeRotation))
	};

	struct BoxColliderComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		REFLECT(
			("Offset", Offset)("FreezeRotation", Size)
			("Density", Density)("Friction", Friction)("Restitution", Restitution)("RestitutionThreshold", RestitutionThreshold)
		)

		void* RuntimeFixture = nullptr;
	};
}