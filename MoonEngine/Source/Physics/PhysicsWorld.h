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

		bool WorldExists() { return m_PhysicsWorld != nullptr; }
		static float Gravity;
	private:
		b2World* m_PhysicsWorld = nullptr;
		int32_t m_VelocityIterations = 6;
		int32_t m_PositionIterations = 2;
		
		void RegisterPhysicsBody(Entity e, const TransformComponent& tc, PhysicsBodyComponent& pb, bool toRegistry = false);
		void UnregisterPhysicsBody(PhysicsBodyComponent& pb, bool toRegistry = false);
		void UpdatePhysicsBodies(Entity e, TransformComponent& tc, const PhysicsBodyComponent& pb);

		friend class Scene;
	};
}