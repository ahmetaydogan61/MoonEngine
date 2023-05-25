#include "mpch.h"
#include "Physics/PhysicsWorld.h"

#include "Engine/Entity.h"

#include "Physics/Collision.h"

#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_contact.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

namespace MoonEngine
{
#pragma region Static Field

	float PhysicsWorld::Gravity = -9.8f, PhysicsWorld::FixedTimestep = 1 / 60.0f;
	int32_t PhysicsWorld::VelocityIterations = 8, PhysicsWorld::PositionIterations = 3, PhysicsWorld::MaxSteps = 5;

	b2BodyType PhysicsWorld::ConvertBodyType(PhysicsBodyComponent::BodyType type)
	{
		switch (type)
		{
			case MoonEngine::PhysicsBodyComponent::BodyType::Static: return b2_staticBody;
			case MoonEngine::PhysicsBodyComponent::BodyType::Dynamic: return b2_dynamicBody;
			case MoonEngine::PhysicsBodyComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		ME_ASSERT(false, "Invalid body type!");
		return b2_staticBody;
	}

#pragma endregion

#pragma region ComponentRegistry

	struct RegisterGroup
	{
		Entity e;
		const TransformComponent& t;
		PhysicsBodyComponent& pb;

		RegisterGroup(Entity e, const TransformComponent& t, PhysicsBodyComponent& pb) :e(e), t(t), pb(pb) {}
	};

	std::vector<RegisterGroup> m_AddRegistry;
	std::vector<PhysicsBodyComponent> m_RemoveRegistry;

#pragma endregion

#pragma region CollisionListener

	class ContactListener : public b2ContactListener
	{
	public:
		std::function<void(void*, void*)> OnContactBegin;
		std::function<void(void*, void*)> m_OnContactEnd;

		void BeginContact(b2Contact* contact) override {
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			// Get the bodies associated with the fixtures
			b2Body* bodyA = fixtureA->GetBody();
			b2Body* bodyB = fixtureB->GetBody();

			if (OnContactBegin)
				OnContactBegin(bodyA->GetUserData().pointer, bodyB->GetUserData().pointer);
		}

		void EndContact(b2Contact* contact) override {
			b2Fixture* fixtureA = contact->GetFixtureA();
			b2Fixture* fixtureB = contact->GetFixtureB();

			// Get the bodies associated with the fixtures
			b2Body* bodyA = fixtureA->GetBody();
			b2Body* bodyB = fixtureB->GetBody();

			if (m_OnContactEnd)
				m_OnContactEnd(bodyA->GetUserData().pointer, bodyB->GetUserData().pointer);
		}
	};

	static ContactListener* s_ContactListener;

#pragma endregion


	void PhysicsWorld::RegisterPhysicsBody(Entity entity, const TransformComponent& transform, PhysicsBodyComponent& pb, bool toRegistry)
	{
		if (toRegistry)
		{
			RegisterGroup rg(entity, transform, pb);
			m_AddRegistry.push_back(rg);
			return;
		}

		b2BodyDef bodyDef;
		bodyDef.type = ConvertBodyType(pb.Type);
		bodyDef.position.Set(transform.Position.x + pb.Offset.x, transform.Position.y + pb.Offset.y);
		bodyDef.angle = transform.Rotation.z;
		bodyDef.linearDamping = pb.LinearDamping;
		bodyDef.angularDamping = pb.AngularDamping;
		bodyDef.gravityScale = pb.GravityScale;
		bodyDef.bullet = pb.IsContinuousDetection;

		b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
		body->SetFixedRotation(pb.FreezeRotation);
		
		Collision* c = new Collision((int)entity.m_ID, &transform, &pb);

		body->GetUserData().pointer = static_cast<void*>(c);

		pb.RuntimeBody = body;

		b2PolygonShape boxShape;
		boxShape.SetAsBox(transform.Scale.x * pb.Size.x, transform.Scale.y * pb.Size.y);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &boxShape;
		fixtureDef.density = pb.Density;
		fixtureDef.friction = pb.Friction;
		fixtureDef.restitution = pb.Restitution;
		fixtureDef.restitutionThreshold = pb.RestitutionThreshold;
		fixtureDef.isSensor = pb.IsTrigger;
		body->CreateFixture(&fixtureDef);
	}

	void PhysicsWorld::UnregisterPhysicsBody(PhysicsBodyComponent& pb, bool toRegistry)
	{
		if (toRegistry)
		{
			m_RemoveRegistry.push_back(pb);
			return;
		}

		b2Body* node = (b2Body*)pb.RuntimeBody;

		delete node->GetUserData().pointer;
		node->GetUserData().pointer = nullptr;

		m_PhysicsWorld->DestroyBody(node);
		pb.RuntimeBody = nullptr;
	}

	void PhysicsWorld::ResetPhysicsBodies(Entity e, TransformComponent& transform, const PhysicsBodyComponent& physicsBody)
	{
		auto* body = (b2Body*)physicsBody.RuntimeBody;

		//Update Body Properties
		body->SetType(ConvertBodyType(physicsBody.Type));
		body->SetFixedRotation(physicsBody.FreezeRotation);
		body->SetLinearDamping(physicsBody.LinearDamping);
		body->SetAngularDamping(physicsBody.AngularDamping);
		body->SetGravityScale(physicsBody.GravityScale);
		body->SetBullet(physicsBody.IsContinuousDetection);

		//Uodate Fixture Properties
		auto* fixture = body->GetFixtureList();
		b2PolygonShape* boxShape = (b2PolygonShape*)fixture->GetShape();
		boxShape->SetAsBox(transform.Scale.x * physicsBody.Size.x, transform.Scale.y * physicsBody.Size.y);
		fixture->SetDensity(physicsBody.Density);
		fixture->SetFriction(physicsBody.Friction);
		fixture->SetRestitution(physicsBody.Restitution);
		fixture->SetRestitutionThreshold(physicsBody.RestitutionThreshold);
		fixture->SetSensor(physicsBody.IsTrigger);

		body->ResetMassData();

		if (body->GetType() == b2_staticBody)
			return;

		const auto& position = body->GetPosition();
		transform.Position.x = (position.x - physicsBody.Offset.x);
		transform.Position.y = position.y - physicsBody.Offset.y;
		transform.Rotation.z = body->GetAngle();
	}

	void PhysicsWorld::UpdatePhysicsBodies(Entity e, TransformComponent& transform, const PhysicsBodyComponent& physicsBody)
	{
		auto* body = (b2Body*)physicsBody.RuntimeBody;

		if (body->GetType() == b2_staticBody)
			return;

		const float oneMinusRatio = 1.f - m_AccumulatorRatio;

		const auto& position = body->GetPosition();
		auto smoothedPosition = b2Vec2();

		smoothedPosition.x = m_AccumulatorRatio * (position.x - physicsBody.Offset.x) + oneMinusRatio * transform.Position.x;
		smoothedPosition.y = m_AccumulatorRatio * (position.y - physicsBody.Offset.y) + oneMinusRatio * transform.Position.y;

		transform.Position.x = smoothedPosition.x;
		transform.Position.y = smoothedPosition.y;
		transform.Rotation.z = m_AccumulatorRatio * body->GetAngle() + oneMinusRatio * transform.Rotation.z;
	}

	void PhysicsWorld::BeginWorld()
	{
		m_PhysicsWorld = new b2World({ 0.0f, Gravity });
		m_Accumulator = 0.0f;
		m_AccumulatorRatio = 0.0f;
	}

	void PhysicsWorld::SetContactListeners(std::function<void(void*, void*)> begin, std::function<void(void*, void*)> end)
	{
		if (m_PhysicsWorld)
		{
			s_ContactListener = new ContactListener();
			s_ContactListener->OnContactBegin = begin;
			s_ContactListener->m_OnContactEnd = end;

			m_PhysicsWorld->SetContactListener(s_ContactListener);
		}
	}

	void PhysicsWorld::StepWorld(float dt, std::function<void()> resetFunction)
	{
		if (m_AddRegistry.size() > 0)
		{
			for (int i = 0; i < m_AddRegistry.size(); i++)
			{
				const RegisterGroup& rg = m_AddRegistry[i];
				RegisterPhysicsBody(rg.e, rg.t, rg.pb);
			}
			m_AddRegistry.clear();
		}

		if (m_RemoveRegistry.size() > 0)
		{
			for (int i = 0; i < m_RemoveRegistry.size(); i++)
				UnregisterPhysicsBody(m_RemoveRegistry[i]);
			m_RemoveRegistry.clear();
		}

		m_Accumulator += dt;

		const int steps = (int)(std::floor(m_Accumulator / FixedTimestep));
		if (steps > 0)
			m_Accumulator -= steps * FixedTimestep;
		const int stepsClamped = std::min(steps, MaxSteps);

		for (int i = 0; i < stepsClamped; i++)
		{
			if (resetFunction)
				resetFunction();
			m_PhysicsWorld->Step(FixedTimestep, VelocityIterations, PositionIterations);
		}

		m_AccumulatorRatio = m_Accumulator / FixedTimestep;

		m_PhysicsWorld->ClearForces();
	}

	void PhysicsWorld::EndWorld()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;

		delete s_ContactListener;
		s_ContactListener = nullptr;
	}
}