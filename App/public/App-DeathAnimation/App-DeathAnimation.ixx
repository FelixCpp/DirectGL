// Project Name : DirectGL
// File Name    : App-DeathAnimation.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:DeathAnimation;

struct DeathAnimation
{
	virtual ~DeathAnimation() = default;
	virtual bool IsFinished() const = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Show() const = 0;
};