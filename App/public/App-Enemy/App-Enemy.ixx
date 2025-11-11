// Project Name : DirectGL
// File Name    : App-Enemy.ixx
// Author       : Felix Busch
// Created Date : 2025/11/11

module;

#include <vector>
#include <memory>

export module App:Enemy;

import DirectGL;

import :DeathAnimation;

struct DelayedDamage
{
	int Amount;
	float Delay;
};

class Enemy
{
public:

	explicit Enemy(
		DGL::Math::Float2 spawnPosition,
		DGL::Math::Float2 size,
		int health,
		float maxSpeed,
		float maxForce
	);

	virtual ~Enemy() = default;

	virtual void Seek(DGL::Math::Float2 targetPosition) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Show() const = 0;

	virtual std::unique_ptr<DeathAnimation> CreateDeathAnimation() const = 0;

	void ApplyDamage(const DelayedDamage& damage);

	bool IsAboutToDie() const;
	bool IsDead() const;

protected:

	void UpdatePendingDamages(float deltaTime);

	void Move(float deltaTime);
	void ApplyForce(DGL::Math::Float2 force);

	virtual void OnDamage(int amount) = 0;

public:

	DGL::Math::Float2 Position;
	DGL::Math::Float2 Velocity;
	DGL::Math::Float2 Acceleration;
	DGL::Math::Float2 Size;

	float MaxSpeed;
	float MaxForce;

	int Health;
	int PendingDamage;

	std::vector<DelayedDamage> PendingDamages;

};