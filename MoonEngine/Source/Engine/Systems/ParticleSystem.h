#pragma once

namespace MoonEngine
{
	enum class EmitterType
	{
		None,
		Box,
	};

	struct ParticleComponent
	{
		glm::vec3 Position = glm::vec3(0.0f);

		//Lifecylce
		bool IsLifetimeConstant = true;
		glm::vec2 Lifetime = glm::vec2(2.0f, 3.5f);

		//Position
		bool IsSpeedConstant = true;
		glm::vec2 Speed = glm::vec2(1.0f, 3.0f);
		glm::vec3 SpawnPosition = glm::vec3(0.0f);
		glm::vec3 SpawnRadius = glm::vec3(0.0f, 0.0f, 0.0f);

		//Emitter Type
		EmitterType EmitterType;
		float RandomDirectionFactor = 0.0f;

		//Scale
		bool IsScale3D = false;
		bool IsScaleConstant = true;
		glm::vec3 ScaleStart = glm::vec3(1.0f);
		glm::vec3 ScaleStartRandom = glm::vec3(1.0f);
		bool IsScaleCycle = false;
		bool IsScaleEndConstant = true;
		glm::vec3 ScaleEnd = glm::vec3(0.0f);
		glm::vec3 ScaleEndRandom = glm::vec3(0.0f);

		//Rotation
		bool IsRotation3D = false;
		bool IsRotationConstant = true;
		glm::vec3 RotationStart = glm::vec3(0.0f);
		glm::vec3 RotationStartRandom = glm::vec3(0.0f);
		bool IsRotationCycle = false;
		bool IsRotationEndConstant = true;
		glm::vec3 RotationEnd = glm::vec3(0.0f);
		glm::vec3 RotationEndRandom = glm::vec3(0.0f);

		//Color
		bool IsColorConstant = true;
		glm::vec4 ColorStart = glm::vec4(1.0f);
		glm::vec4 ColorStartRandom = glm::vec4(1.0f);
		bool IsColorCycle = false;
		bool IsColorEndConstant = true;
		glm::vec4 ColorEnd = glm::vec4(1.0f);
		glm::vec4 ColorEndRandom = glm::vec4(1.0f);
	};

	struct Particle
	{
		bool IsActive = false;

		float Lifetime = 0.0f;
		float LifeElapsed = 0.0f;
		float Speed = 0.0f;

		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Direction = glm::vec3(0.0f);

		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 RotationStart = glm::vec3(0.0f);
		glm::vec3 RotationEnd = glm::vec3(0.0f);

		glm::vec3 Scale = glm::vec3(0.0f);
		glm::vec3 ScaleStart = glm::vec3(0.0f);
		glm::vec3 ScaleEnd = glm::vec3(0.0f);

		glm::vec4 Color = glm::vec4(0.0f);
		glm::vec4 ColorStart = glm::vec4(0.0f);
		glm::vec4 ColorEnd = glm::vec4(0.0f);
	};

	struct ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem() = default;

		void Update(float dt, const ParticleComponent& particle);
		void Spawn(const ParticleComponent& particle);
		void UpdateParticles(float dt);

		bool Looping = true;
		float Duration = 5.0f;
		float ParticlePerSecond = 5.0f;
		float PerSecond = 0.2f;

		bool IsPlaying() { return m_IsPlaying; }
		bool IsPaused() { return m_IsPaused; }

		void Play() { m_IsPlaying = true; m_IsPaused = false; }
		void Pause() { m_IsPlaying = false; m_IsPaused = true; }
		void Stop();

		void DrawGui();
	private:
		bool m_IsPlaying = false;
		bool m_IsPaused = false;

		float m_PerSecondTimer = 0.0f;
		float m_DurationTimer = 0.0f;
		uint32_t m_SpawnCount = 0;

		std::vector<Particle> m_Particles;
		uint32_t m_PoolSize = 1000;
		uint32_t m_PoolIndex = 0;
		uint32_t m_AliveParticles = 0;
	};
}