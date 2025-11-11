// Project Name : DirectGL
// File Name    : App-TankEnemy.ixx
// Author       : Felix Busch
// Created Date : 2025/11/11

export module App:TankEnemy;

import DirectGL;

import :Enemy;

class TankEnemy : public Enemy
{
public:

	explicit TankEnemy(DGL::Math::Float2 position);

	void Seek(DGL::Math::Float2 targetPosition) override;
	void Update(float deltaTime) override;
	void Show() const override;

	std::unique_ptr<DeathAnimation> CreateDeathAnimation() const override;

public:

	float SpawnAnimationProgress;
	float HealthFlashProgress;

protected:

	void OnDamage(int amount) override;

};