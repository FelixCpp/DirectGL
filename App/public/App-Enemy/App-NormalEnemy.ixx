// Project Name : DirectGL
// File Name    : App-NormalEnemy.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module App:NormalEnemy;

import DirectGL;

import :Enemy;

class NormalEnemy : public Enemy
{
public:

	explicit NormalEnemy(DGL::Math::Float2 position);

	void Seek(DGL::Math::Float2 target) override;
	void Update(float deltaTime) override;
	void Show() const override;

	std::unique_ptr<DeathAnimation> CreateDeathAnimation() const override;

public:

	float	SpawnAnimationProgress;
	float	HealthFlashProgress;

protected:

	void OnDamage(int amount) override;

};