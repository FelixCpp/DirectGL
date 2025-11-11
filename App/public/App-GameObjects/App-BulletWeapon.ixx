// Project Name : DirectGL
// File Name    : App-BulletWeapon.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <vector>
#include <memory>

export module App:BulletWeapon;

import DirectGL;

import :Enemy;
import :PositionProvider;

class BulletWeapon
{
public:

	struct Bullet
	{
		DGL::Math::Float2 Position;
		DGL::Math::Float2 Velocity;
		float LifeTime;
		float TimeAlive;
	};

	struct SortedEnemy
	{
		Enemy* Enemy;
		DGL::Math::Float2 Difference;
		float Distance;
		float DistanceSquared;
	};

	explicit BulletWeapon(const PositionProvider& emissionPosition, float shootInterval, float shootRadius);

	void Shoot(std::vector<std::unique_ptr<Enemy>>& enemies);
	void Update(float deltaTime);
	void Show() const;

public:

	std::vector<SortedEnemy> GetSortedEnemies(std::vector<std::unique_ptr<Enemy>>& enemies) const;

	const PositionProvider*			EmissionPosition;
	std::vector<Bullet>				Bullets;
	float 							ShootInterval;
	float 							TimeElapsedSinceLastShot;
	float							ShootRadius;
	float							SpawnAnimationProgress;

};