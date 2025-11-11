// Project Name : DirectGL
// File Name    : App-ParticleDeathAnimation.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <vector>

export module App:ParticleDeathAnimation;

import DirectGL;

import :DeathAnimation;

class ParticleDeathAnimation : public DeathAnimation
{
public:

	struct Particle
	{
		DGL::Math::Float2 Position;
		DGL::Math::Float2 Velocity;
		float LifeTime;
		float TimeAlive;
	};

	explicit ParticleDeathAnimation(
		const DGL::Math::Float2& spawnPosition,
		size_t numParticle,
		DGL::Math::Float3 color
	);

	bool IsFinished() const override;
	void Update(float deltaTime) override;
	void Show() const override;

public:

	std::vector<Particle> Particles;
	DGL::Math::Float3 Color;
	float MaxLifetime;
	float ElapsedTime;

};