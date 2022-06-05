#include "mpch.h"
#include "ParticleSystem.h"

#include "Renderer/Renderer.h"

#include "Engine/Components.h"

namespace MoonEngine
{
	int ParticleSystem::AWAKECOUNT = 0;

	ParticleSystem::ParticleSystem()
	{
		m_ParticlePool.resize(100000);
		m_PoolIndex = m_ParticlePool.size() - 1;
	}

	void ParticleSystem::Update()
	{
		float deltaTime = Time::DeltaTime();

		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeElapsed >= particle.Lifetime)
			{
				particle.Active = false;
				AWAKECOUNT--;
				continue;
			}

			particle.LifeElapsed += deltaTime;
			particle.Position += particle.Direction * deltaTime;

			float life = particle.LifeElapsed / particle.Lifetime;
			glm::vec4 color = Maths::Lerp(particle.ColorStart, particle.ColorEnd, life * particle.ColorChangeSpeed);

			if (particle.Size.x > 0.0f && particle.Size.y > 0.0f)
				particle.Size = Maths::Lerp(particle.SizeStart, particle.SizeEnd, life * particle.SizeChangeSpeed);
			else
			{
				particle.Active = false;
				continue;
			}

			glm::mat4 transform =
				glm::translate(glm::mat4(1.0f), particle.Position)
				* glm::scale(glm::mat4(1.0f), particle.Size);

			if(particle.Texture == nullptr)
				Renderer::DrawQuad(transform, color);
			else
				Renderer::DrawQuad(transform, color, particle.Texture);
		}
	}

	void ParticleSystem::Emit(const ParticleComponent& component, glm::vec3 position)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		AWAKECOUNT++;

		//Position
		particle.Position = position;
		particle.Direction = component.Direction;
		particle.Direction.x += component.DirectionVelocity.x * (Maths::RandomFloat(-0.5f, 0.5f));
		particle.Direction.y += component.DirectionVelocity.y * (Maths::RandomFloat(-0.5f, 0.5f));
		particle.Direction.z += component.DirectionVelocity.z * (Maths::RandomFloat(-0.5f, 0.5f));

		//Size
		particle.Size = component.SizeStart;
		particle.SizeChangeSpeed = component.SizeChangeSpeed;
		particle.SizeStart = component.SizeStart;
		particle.SizeEnd = component.SizeEnd;

		// Color
		particle.Texture = component.Texture;
		particle.ColorStart = component.ColorStart;
		particle.ColorEnd = component.ColorEnd;
		particle.ColorChangeSpeed = component.ColorChangeSpeed;

		//Lifecycle
		particle.Lifetime = component.Lifetime;
		particle.LifeElapsed = 0.0f;

		m_PoolIndex--;
		if (m_PoolIndex == 0)
			m_PoolIndex = m_ParticlePool.size() - 1;
	}
}
