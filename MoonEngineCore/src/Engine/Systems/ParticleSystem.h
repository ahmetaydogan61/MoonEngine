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
			//Position
			glm::vec3 Position;
			glm::vec3 Direction;

			//Size
			glm::vec3 Size, SizeStart, SizeEnd;
			float SizeChangeSpeed;

			//Color
			Ref<Texture> Texture;
			glm::vec4 ColorStart, ColorEnd;
			float ColorChangeSpeed;

			//Lifecycle
			bool Active = false;
			float Lifetime = 1.0f;
			float LifeElapsed = 0.0f;
		};
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex;
	public:
		static int AWAKECOUNT;
		ParticleSystem();

		void Emit(const ParticleComponent& component, glm::vec3 position);
		void Update();
	};
}