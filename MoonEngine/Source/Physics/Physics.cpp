#include "mpch.h"
#include "Physics/Physics.h"

#include <box2d/b2_body.h>

namespace MoonEngine
{
	void PhysicsBodyComponent::SetPosition(const glm::vec2& position)
	{
		if (RuntimeBody)
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->SetTransform({ position.x, position.y }, 0.0f);
		}
	}

	void PhysicsBodyComponent::AddForce(const glm::vec2& force, const glm::vec2& position)
	{
		if (RuntimeBody)
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyForce({ force.x, force.y }, body->GetWorldPoint({ position.x, position.y }), true);
		}
	}

	void PhysicsBodyComponent::AddImpulse(const glm::vec2& force, const glm::vec2& position)
	{
		if (RuntimeBody)
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyLinearImpulse({ force.x, force.y }, body->GetWorldPoint({ position.x, position.y }), true);
		}
	}

	void PhysicsBodyComponent::AddTorque(float torque)
	{
		if (RuntimeBody)
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyTorque(torque, true);
		}
	}

	void PhysicsBodyComponent::AddAngularImpulse(float impulse)
	{
		if (RuntimeBody)
		{
			b2Body* body = (b2Body*)RuntimeBody;
			body->ApplyAngularImpulse(impulse, true);
		}
	}
}