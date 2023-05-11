#pragma once
#include "Engine/Components.h"

class b2World;

namespace MoonEngine
{
	class Entity;
	
	class PhysicsWorld
	{
	public:
		//Create World (call when simulation starts)
		void BeginWorld();
		//Update World, ResetFunction: Reset all phyiscs components for smoothing.
		void StepWorld(float dt, std::function<void()> resetFunction = nullptr);
		//Destroy world (call when simulation ends)
		void EndWorld();

		bool WorldExists() { return m_PhysicsWorld != nullptr; }
		static float Gravity;
		static int32_t VelocityIterations;
		static int32_t PositionIterations;
		static int32_t MaxSteps;
		static float FixedTimestep;
	private:
		b2World* m_PhysicsWorld = nullptr;
		
		void RegisterPhysicsBody(Entity e, const TransformComponent& tc, PhysicsBodyComponent& pb, bool toRegistry = false);
		void UnregisterPhysicsBody(PhysicsBodyComponent& pb, bool toRegistry = false);

		void UpdatePhysicsBodies(Entity e, TransformComponent& tc, const PhysicsBodyComponent& pb);
		void ResetPhysicsBodies(Entity e, TransformComponent& tc, const PhysicsBodyComponent& pb);

		float m_Accumulator = 0;
		float m_AccumulatorRatio = 0;

		friend class Scene;
	};
}