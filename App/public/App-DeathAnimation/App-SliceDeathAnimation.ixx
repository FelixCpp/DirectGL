// Project Name : DirectGL
// File Name    : App-SliceDeathAnimation.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <array>

export module App:SliceDeathAnimation;

import :DeathAnimation;

import DirectGL;

class SliceDeathAnimation : public DeathAnimation
{
public:

	explicit SliceDeathAnimation(DGL::Math::Float2 position, DGL::Math::Float2 size, float duration);

	bool IsFinished() const override;
	void Update(float deltaTime) override;
	void Show() const override;

public:

	struct Slice
	{
		DGL::Math::Float2 Position;
		DGL::Math::Float2 Velocity;
		DGL::Math::Float2 Size;
		float LifeTime;
		float TimeAlive;
	};

	std::array<Slice, 4> Slices;

	float ElapsedTime;
	float Duration;

};