#pragma once

namespace MoonEngine
{
	class Texture;

	enum class EmitterType
	{
		None,
		Box,
		Cone
	};

	enum class SortMode
	{
		OldestInFront,
		YoungestInFront
	};

	struct ParticleBody
	{
		//Lifecylce
		bool IsLifetimeConstant = true;
		glm::vec2 Lifetime = glm::vec2(2.0f, 3.5f);

		//Position
		bool IsSpeedConstant = true;
		glm::vec2 Speed = glm::vec2(1.0f, 3.0f);
		glm::vec3 SpawnPosition = glm::vec3(0.0f);
		glm::vec3 SpawnRadius = glm::vec3(1.0f, 1.0f, 0.0f);

		//Emiiter
		float RandomDirectionFactor = 1.0f;
		float DirectionRadiusFactor = 1.0f;

		//Scale
		bool IsScale3D = false;
		bool IsScaleConstant = true;
		glm::vec3 ScaleStart = glm::vec3(1.0f);
		glm::vec3 ScaleStartRandom = glm::vec3(1.0f);
		bool IsScaleCycle = true;
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

		//Rendering
		Shared<Texture> Texture;
		bool IsColorConstant = true;
		glm::vec4 ColorStart = glm::vec4(1.0f);
		glm::vec4 ColorStartRandom = glm::vec4(1.0f);
		bool IsColorCycle = false;
		bool IsColorEndConstant = true;
		glm::vec4 ColorEnd = glm::vec4(1.0f);
		glm::vec4 ColorEndRandom = glm::vec4(1.0f);

		REFLECT
		(
			("IsLifetimeConstant", IsLifetimeConstant)("Lifetime", Lifetime)
			("IsSpeedConstant", IsSpeedConstant)("Speed", Speed)("SpawnPosition", SpawnPosition)("SpawnRadius", SpawnRadius)
			("RandomDirectionFactor", RandomDirectionFactor)("DirectionRadiusFactor", DirectionRadiusFactor)

			("IsScale3D", IsScale3D)("IsScaleConstant", IsScaleConstant)("ScaleStart", ScaleStart)("ScaleStartRandom", ScaleStartRandom)
			("IsScaleCycle", IsScaleCycle)("IsScaleEndConstant", IsScaleEndConstant)("ScaleEnd", ScaleEnd)("ScaleEndRandom", ScaleEndRandom)

			("IsRotation3D", IsRotation3D)("IsRotationConstant", IsRotationConstant)("RotationStart", RotationStart)("RotationStartRandom", RotationStartRandom)
			("IsRotationCycle", IsRotationCycle)("IsRotationEndConstant", IsRotationEndConstant)("RotationEnd", RotationEnd)("RotationEndRandom", RotationEndRandom)

			("Texture", Texture)
			("IsColorConstant", IsColorConstant)("ColorStart", ColorStart)("ColorStartRandom", ColorStartRandom)
			("IsColorCycle", IsColorCycle)("IsColorEndConstant", IsColorEndConstant)("ColorEnd", ColorEnd)("ColorEndRandom", ColorEndRandom)
		)
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

		Shared<Texture> Texture;
		glm::vec4 Color = glm::vec4(0.0f);
		glm::vec4 ColorStart = glm::vec4(0.0f);
		glm::vec4 ColorEnd = glm::vec4(0.0f);
	};


	struct ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem() = default;


		void Spawn(const ParticleBody& particle, const glm::vec3& position);
		
		void UpdateEmitter(float dt, const ParticleBody& particle, const glm::vec3& position);
		void UpdateParticles(float dt);
		void DrawParticles(int entityId);

		SortMode SortMode = SortMode::YoungestInFront;
		EmitterType EmitterType = EmitterType::Cone;
		bool PlayOnAwake = true;
		bool Looping = true;
		float Duration = 5.0f;
		float ParticlePerSecond = 5.0f;
		float ParticlePerUnit = 0.0f;
		int Layer = 0;

		bool IsPlaying() { return m_IsPlaying; }
		bool IsPaused() { return m_IsPaused; }
		uint32_t Size() { return m_PoolSize; }

		void Resize(uint32_t newSize);
		void Play() { m_IsPlaying = true; m_IsPaused = false; }
		void Pause() { m_IsPlaying = false; m_IsPaused = true; }
		void Stop();

		REFLECT(("SortMode", SortMode)("EmitterType", EmitterType)("PlayOnAwake", PlayOnAwake)("Looping", Looping)("Duration", Duration)("ParticlePerSecond", ParticlePerSecond)("Layer", Layer))
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

		glm::vec3 m_LastPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	};
}