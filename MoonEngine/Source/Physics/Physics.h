#pragma once
#include "Event/Action.h"
#include "Physics/Collision.h"

namespace MoonEngine
{
	struct PhysicsBodyComponent
	{
		//Body
		enum class BodyType { Static, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FreezeRotation = false;
		bool IsContinuousDetection = false; // only dynamic (box2d->bullet)
		bool IsTrigger = false; //(box2d->sensor)

		float LinearDamping = 0.0f; // 0 - Inf
		float AngularDamping = 0.01f; // 0 - Inf
		float GravityScale = 1.0f;

		void* RuntimeBody = nullptr;

		//Fixture
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		//Fixture Material
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void AddForce(const glm::vec2& force, const glm::vec2& position);
		void AddImpulse(const glm::vec2& force, const glm::vec2& position);
		void AddTorque(float torque);
		void AddAngularImpulse(float impulse);

		Action<Collision*> OnCollisionEnter;
		Action<Collision*> OnCollisionExit;

		REFLECT(
			("Type", Type)("FreezeRotation", FreezeRotation)("ContinuousDetection", IsContinuousDetection)("IsTrigger", IsTrigger)
			("LinearDamping", LinearDamping)("AngularDamping", AngularDamping)("GravityScale", GravityScale)
			("Offset", Offset)("Size", Size)
			("Density", Density)("Friction", Friction)("Restitution", Restitution)("RestitutionThreshold", RestitutionThreshold)
		)
	};
}