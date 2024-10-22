#include "mpch.h"
#include "Engine/Systems/ParticleSystem.h"

#include "Core/Time.h"
#include "Renderer/Renderer.h"
#include "Renderer/Texture.h"

#include "Utils/Maths.h"

#include <imgui.h>

namespace MoonEngine
{
	ParticleSystem::ParticleSystem()
	{
		m_Particles.resize(m_PoolSize);
	}

	void ParticleSystem::UpdateEmitter(float dt, const ParticleBody& particle, const glm::vec3& position)
	{
		if (!m_IsPlaying)
			return;

		m_DurationTimer += dt;
		if (m_DurationTimer >= Duration && !Looping)
		{
			Stop();
			return;
		}

		if (ParticlePerSecond > 0.0f)
		{
			m_PerSecondTimer += dt;
			float spawnPerSecond = 1.0f / ParticlePerSecond;
			if (m_PerSecondTimer >= spawnPerSecond)
			{
				m_SpawnCount++;
				m_PerSecondTimer = 0.0f;
			}
		}
		else
			m_PerSecondTimer = 0.0f;

		if (ParticlePerUnit > 0)
		{
			float distance = glm::distance(m_LastPosition, position);
			float spawnPerUnit = 1 / ParticlePerUnit;

			if (distance >= spawnPerUnit)
				m_SpawnCount++;
		}

		m_LastPosition = position;

		for (uint32_t i = 0; i < m_SpawnCount; i++)
			Spawn(particle, position);

		m_SpawnCount = 0;
	}

	void ParticleSystem::UpdateParticles(float dt)
	{
		if (!m_IsPlaying && !m_IsPaused)
			return;

		m_AliveParticles = 0;

		if (m_PoolSize <= 0)
			return;

		for (uint32_t i = 0; i < m_PoolSize; i++)
		{
			Particle& particle = m_Particles[i];

			if (!particle.IsActive)
				continue;

			if (!m_IsPaused)
			{
				float normalizedLife = particle.LifeElapsed / particle.Lifetime;

				particle.Position += particle.Direction * particle.Speed * dt;
				particle.Scale = Maths::Lerp(particle.ScaleStart, particle.ScaleEnd, normalizedLife);
				particle.Rotation = Maths::Lerp(particle.RotationStart, particle.RotationEnd, normalizedLife);
				particle.Color = Maths::Lerp(particle.ColorStart, particle.ColorEnd, normalizedLife);

				particle.LifeElapsed += dt;
				if (particle.LifeElapsed >= particle.Lifetime)
				{
					particle.LifeElapsed = particle.Lifetime;
					particle.IsActive = false;
				}
			}
			m_AliveParticles++;
		}
	}

	void ParticleSystem::DrawParticles(int entityId)
	{
		if (!m_IsPlaying && !m_IsPaused)
			return;

		if (SortMode == SortMode::YoungestInFront)
		{
			for (uint32_t i = 0; i < m_PoolSize; i++)
			{
				Particle& particle = m_Particles[i];
				if (!particle.IsActive)
					continue;
				Renderer::DrawEntity(particle.Position, particle.Scale, particle.Rotation, particle.Color, particle.Texture, Layer, { 1.0f, 1.0f }, entityId);
			}
		}
		else if (SortMode == SortMode::OldestInFront)
		{
			for (uint32_t i = m_PoolSize - 1; i > 0; i--)
			{
				Particle& particle = m_Particles[i];
				if (!particle.IsActive)
					continue;
				Renderer::DrawEntity(particle.Position, particle.Scale, particle.Rotation, particle.Color, particle.Texture, Layer, { 1.0f, 1.0f }, entityId);
			}
		}
	}

	void ParticleSystem::Spawn(const ParticleBody& p, const glm::vec3& position)
	{
		if (m_Particles.size() <= 0)
		{
			m_PoolIndex = 0;
			return;
		}

		Particle& particle = m_Particles[m_PoolIndex];
		particle.IsActive = true;
		particle.LifeElapsed = 0.0f;

		if (p.IsLifetimeConstant)
			particle.Lifetime = p.Lifetime[0];
		else
			particle.Lifetime = Maths::RandomFloat(p.Lifetime[0], p.Lifetime[1]);

		if (p.IsSpeedConstant)
			particle.Speed = p.Speed[0];
		else
			particle.Speed = Maths::RandomFloat(p.Speed[0], p.Speed[1]);

		particle.Position = position;
		particle.Direction = glm::vec3(0.0f, 1.0f, 0.0f);

		switch (EmitterType)
		{
			case EmitterType::Box:
			{
				const glm::vec3& spawnRadius = p.SpawnRadius * 0.5f;
				const glm::vec3& spawnPos = glm::vec3(Maths::RandomFloat(-spawnRadius.x, spawnRadius.x),
													  Maths::RandomFloat(-spawnRadius.y, spawnRadius.y),
													  0.0f);

				particle.Position += p.SpawnPosition + spawnPos;

				if (p.RandomDirectionFactor > 0.0f)
				{
					particle.Direction = glm::vec3(Maths::RandomFloat(-1.0f, 1.0f),
												   Maths::RandomFloat(-1.0f, 1.0f),
												   Maths::RandomFloat(-1.0f, 1.0f));
				}
				break;
			}
			case EmitterType::Cone:
			{
				float spawnRadius = p.SpawnRadius.x * 0.5f;
				const glm::vec3& spawnPos = glm::vec3(Maths::RandomFloat(-spawnRadius - 0.1f, spawnRadius + 0.1f), 0.0f, 0.0f);
				particle.Position += p.SpawnPosition + spawnPos;
				int dir = 0;
				if (spawnPos.x <= -0.01f)
					dir = -1;
				else if (spawnPos.x >= 0.01f)
					dir = 1;

				float randomDirX = spawnRadius + p.DirectionRadiusFactor * 0.5f;
				particle.Direction.x = Maths::RandomFloat(0.0f, dir * randomDirX);
				particle.Direction.y += p.SpawnRadius.y * 0.5f;
				break;
			}
			default:
				break;
		}
#pragma region Scale

		if (p.IsScaleConstant)
		{
			if (p.IsScale3D)
				particle.ScaleStart = p.ScaleStart;
			else
				particle.ScaleStart = glm::vec3(p.ScaleStart[0]);
		}
		else
		{
			if (p.IsScale3D)
			{
				particle.ScaleStart = glm::vec3(
					Maths::RandomFloat(p.ScaleStart.x, p.ScaleStartRandom.x),
					Maths::RandomFloat(p.ScaleStart.y, p.ScaleStartRandom.y),
					Maths::RandomFloat(p.ScaleStart.z, p.ScaleStartRandom.z));
			}
			else
				particle.ScaleStart = glm::vec3(Maths::RandomFloat(p.ScaleStart.x, p.ScaleStartRandom.x));
		}

		if (p.IsScaleCycle)
		{
			if (p.IsScaleEndConstant)
			{
				if (p.IsScale3D)
					particle.ScaleEnd = p.ScaleEnd;
				else
					particle.ScaleEnd = glm::vec3(p.ScaleEnd[0]);
			}
			else
			{
				if (p.IsScale3D)
				{
					particle.ScaleEnd = glm::vec3(
						Maths::RandomFloat(p.ScaleEnd.x, p.ScaleEndRandom.x),
						Maths::RandomFloat(p.ScaleEnd.y, p.ScaleEndRandom.y),
						Maths::RandomFloat(p.ScaleEnd.z, p.ScaleEndRandom.z));
				}
				else
					particle.ScaleEnd = glm::vec3(Maths::RandomFloat(p.ScaleEnd.x, p.ScaleEndRandom.x));
			}
		}
		else
			particle.ScaleEnd = particle.ScaleStart;

		particle.Scale = particle.ScaleStart;

#pragma endregion

#pragma region Rotation

		if (p.IsRotationConstant)
		{
			if (p.IsRotation3D)
				particle.RotationStart = p.RotationStart;
			else
				particle.RotationStart = glm::vec3(0.0f, 0.0f, p.RotationStart.z);
		}
		else
		{
			if (p.IsRotation3D)
			{
				particle.RotationStart = glm::vec3(
					Maths::RandomFloat(p.RotationStart.x, p.RotationStartRandom.x),
					Maths::RandomFloat(p.RotationStart.y, p.RotationStartRandom.y),
					Maths::RandomFloat(p.RotationStart.z, p.RotationStartRandom.z));
			}
			else
				particle.RotationStart = glm::vec3(0.0f, 0.0f, Maths::RandomFloat(p.RotationStart.z, p.RotationStartRandom.z));
		}

		if (p.IsRotationCycle)
		{
			if (p.IsRotationEndConstant)
			{
				if (p.IsRotation3D)
					particle.RotationEnd = p.RotationEnd;
				else
					particle.RotationEnd = glm::vec3(0.0f, 0.0f, p.RotationEnd.z);
			}
			else
			{
				if (p.IsRotation3D)
				{
					particle.RotationEnd = glm::vec3(
						Maths::RandomFloat(p.RotationEnd.x, p.RotationEndRandom.x),
						Maths::RandomFloat(p.RotationEnd.y, p.RotationEndRandom.y),
						Maths::RandomFloat(p.RotationEnd.z, p.RotationEndRandom.z));
				}
				else
					particle.RotationEnd = glm::vec3(0.0f, 0.0f, Maths::RandomFloat(p.RotationEnd.z, p.RotationEndRandom.z));
			}
		}
		else
			particle.RotationEnd = particle.RotationStart;

		particle.Rotation = particle.RotationStart;

#pragma endregion

#pragma region Rendering
		particle.Texture = p.Texture;

		if (p.IsColorConstant)
			particle.ColorStart = p.ColorStart;
		else
		{
			float random = Maths::RandomFloat(0.0f, 1.0f);
			float red = p.ColorStart.x + ((p.ColorStartRandom.x - p.ColorStart.x) * random);
			float green = p.ColorStart.y + ((p.ColorStartRandom.y - p.ColorStart.y) * random);
			float blue = p.ColorStart.z + ((p.ColorStartRandom.z - p.ColorStart.z) * random);
			particle.ColorStart = glm::vec4(red, green, blue, Maths::RandomFloat(p.ColorStart.w, p.ColorStartRandom.w));
		}

		if (p.IsColorCycle)
		{
			if (p.IsColorEndConstant)
				particle.ColorEnd = p.ColorEnd;
			else
			{
				float random = Maths::RandomFloat(0.0f, 1.0f);
				float red = p.ColorEnd.x + ((p.ColorEndRandom.x - p.ColorEnd.x) * random);
				float green = p.ColorEnd.y + ((p.ColorEndRandom.y - p.ColorEnd.y) * random);
				float blue = p.ColorEnd.z + ((p.ColorEndRandom.z - p.ColorEnd.z) * random);
				particle.ColorEnd = glm::vec4(red, green, blue, Maths::RandomFloat(p.ColorEnd.w, p.ColorEndRandom.w));
			}
		}
		else
			particle.ColorEnd = particle.ColorStart;

		particle.Color = particle.ColorStart;

#pragma endregion

		m_PoolIndex++;
		if (m_PoolIndex >= m_PoolSize)
			m_PoolIndex = 0;
	}

	void ParticleSystem::Resize(uint32_t newSize)
	{
		bool wasPlaying = m_IsPlaying;
		bool wasPaused = m_IsPaused;

		m_PoolSize = newSize;
		Stop();

		if (wasPlaying)
			Play();
		if (wasPaused)
			Pause();
	}


	void ParticleSystem::Stop()
	{
		m_AliveParticles = 0;
		m_PerSecondTimer = 0.0f;
		m_DurationTimer = 0.0f;
		m_SpawnCount = 0;
		m_PoolIndex = 0;
		m_IsPlaying = false;
		m_IsPaused = false;
		m_Particles.clear();
		m_Particles.resize(m_PoolSize);
	}
}