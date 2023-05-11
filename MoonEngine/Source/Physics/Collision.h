#pragma once

namespace MoonEngine
{
	struct TransformComponent;
	struct PhysicsBodyComponent;

	struct Collision
	{
		int EntityId;
		const TransformComponent* Transform;
		PhysicsBodyComponent* PhysicsBody;
	
		Collision(int e, const TransformComponent* t, PhysicsBodyComponent* pb)
			:EntityId(e), Transform(t), PhysicsBody(pb)
		{}
	};
}
