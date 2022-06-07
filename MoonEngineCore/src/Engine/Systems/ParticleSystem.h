#pragma once

namespace MoonEngine
{
	struct ParticleComponent;
	class Texture;

	class ParticleSystem
	{
	private:
		struct Particle
		{
			//Lifecycle
			bool Active = false;
			float Lifetime, LifeElapsed;

			//Position
			glm::vec3 Position, Direction;

			//Size
			glm::vec3 SizeStart, SizeEnd;
			float SizeChangeSpeed;

			//Color
			Ref<Texture> Texture;
			glm::vec4 ColorStart, ColorEnd;
			float ColorChangeSpeed;

			bool UsePhysics;
			float mass;
			float gravity;
			glm::vec3 velocity;

		};
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex;
	public:
		ParticleSystem();

		void Spawn(const ParticleComponent& component, glm::vec3 position);
		void Update();
		void ResizePool(int size) { m_ParticlePool.resize(size); m_PoolIndex = m_ParticlePool.size() - 1; }
		int PoolSize() { return m_ParticlePool.size(); }
	};
}