module;

#include <utility>

module App;

import :ParticleDeathAnimation;

ParticleDeathAnimation::ParticleDeathAnimation(
	const DGL::Math::Float2& spawnPosition,
	const size_t numParticle,
	const DGL::Math::Float3 color
) : MaxLifetime{ 0.0f },
	ElapsedTime{ 0.0f },
	Color{ color }
{
	Particles.reserve(numParticle);
	for (size_t i = 0; i < numParticle; ++i)
	{
		// Get a random direction vector
		const DGL::Math::Float2 randomDirection = DGL::Math::Float2::Random();

		// Compute a random speed and lifetime for the particle
		const float randomSpeed = DGL::Math::Random(25.0f, 50.0f);
		const float randomLifeTime = DGL::Math::Random(1.f, 1.25f);

		// Update the maximum lifetime of the animation
		MaxLifetime = std::max(MaxLifetime, randomLifeTime);
		Particles.push_back(Particle {
			.Position = spawnPosition,
			.Velocity = randomDirection * randomSpeed,
			.LifeTime = randomLifeTime,
			.TimeAlive = 0.0f
		});
	}
}

bool ParticleDeathAnimation::IsFinished() const
{
	return ElapsedTime >= MaxLifetime;
}

void ParticleDeathAnimation::Update(const float deltaTime)
{
	ElapsedTime += deltaTime;

	// Update all particles
	for (Particle& particle : Particles)
	{
		particle.TimeAlive += deltaTime;
		particle.Position += particle.Velocity * deltaTime;
	}
}

void ParticleDeathAnimation::Show() const
{
	DGL::PushStyle();
	{
		DGL::SetStrokeWeight(2.0f);
		for (const Particle& particle : Particles)
		{
			const float progress = particle.TimeAlive / particle.LifeTime;
			if (progress >= 1.0f)
				continue;

			const float alpha = 1.0f - progress;
			DGL::SetFillColor({ Color, alpha });
			DGL::SetStrokeColor({ COLOR_WHITE, alpha });
			DGL::Circle(particle.Position.X, particle.Position.Y, 4.0f);
		}
	}
	DGL::PopStyle();
}