#pragma once
#include "Engine/Components.h"

class b2World;

namespace MoonEngine
{
	class Entity;

	class PhysicsWorld
	{
	public:
		void BeginWorld();
		void EndWorld();
		void StepWorld(float dt);

		static float Gravity;
	private:
		b2World* m_PhysicsWorld;
		int32_t m_VelocityIterations = 6;
		int32_t m_PositionIterations = 2;

		void RegisterPhysicsBody(Entity& e, const TransformComponent& tc, PhysicsBodyComponent& pb);
		void UnregisterPhysicsBody(Entity& e, PhysicsBodyComponent& pb);
		void UpdatePhysicsBodies(Entity e, TransformComponent& tc, const PhysicsBodyComponent& pb);

		friend class Scene;
	};
}