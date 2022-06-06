#include "mpch.h"
#include "ParticleSystem.h"

#include "Renderer/Renderer.h"

#include "Engine/Components.h"

namespace MoonEngine
{
	ParticleSystem::ParticleSystem()
	{
		m_ParticlePool.resize(1000);
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
				continue;
			}
			
			particle.Position += particle.Direction * deltaTime;
			
			float life = particle.LifeElapsed / particle.Lifetime;
			glm::vec3 size;
			glm::vec4 color;
			
			if (particle.LifeElapsed < particle.Lifetime)
			{
				size = Maths::Lerp(particle.SizeStart, particle.SizeEnd, Maths::Clamp(life * particle.SizeChangeSpeed, 0.0f, 1.0f));
				color = Maths::Lerp(particle.ColorStart, particle.ColorEnd, Maths::Clamp(life * particle.ColorChangeSpeed, 0.0f, 1.0f));
			}
			else
			{
				color = particle.ColorEnd;
				size = particle.SizeEnd;
			}

			glm::mat4 transform =
				glm::translate(glm::mat4(1.0f), particle.Position)
				* glm::scale(glm::mat4(1.0f), size);

			if(particle.Texture == nullptr)
				Renderer::DrawQuad(transform, color);
			else
				Renderer::DrawQuad(transform, color, particle.Texture);

			particle.LifeElapsed += deltaTime;
		}
	}

	void ParticleSystem::Spawn(const ParticleComponent& component, glm::vec3 position)
	{
		if (m_ParticlePool.size() <= 0)
		{
			m_PoolIndex = 0;
			return;
		}

		Particle& particle = m_ParticlePool[m_PoolIndex];
		
		//Lifecycle
		particle.Active = true;
		particle.Lifetime = component.Lifetime;
		particle.LifeElapsed = 0.0f;

		//Position
		particle.Position = position;
		particle.Direction = component.Direction;
		particle.Direction.x += component.DirectionVelocity.x * (Maths::RandomFloat(-0.5f, 0.5f));
		particle.Direction.y += component.DirectionVelocity.y * (Maths::RandomFloat(-0.5f, 0.5f));
		particle.Direction.z += component.DirectionVelocity.z * (Maths::RandomFloat(-0.5f, 0.5f));

		//Size
		particle.SizeChangeSpeed = component.SizeChangeSpeed;
		particle.SizeStart = component.SizeStart;
		particle.SizeEnd = component.SizeEnd;

		// Color
		particle.Texture = component.Texture;
		particle.ColorStart = component.ColorStart;
		particle.ColorEnd = component.ColorEnd;
		particle.ColorChangeSpeed = component.ColorChangeSpeed;
		
		m_PoolIndex--;
		if (m_PoolIndex == 0)
			m_PoolIndex = m_ParticlePool.size() - 1;
	}
}
