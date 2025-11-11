// Project Name : DirectGL
// File Name    : App-EnemySpawner.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

module;

#include <memory>
#include <vector>

export module App:EnemySpawner;

import :Enemy;
import :PositionProvider;
import :DeathAnimation;

class EnemySpawner
{
public:

	explicit EnemySpawner(
		const PositionProvider& positionProvider,
		float spawnInterval,
		float spawnRadius
	);

	void Update(float deltaTime);
	void Show() const;

public:

	const PositionProvider* SpawnCenter;

	std::vector<std::unique_ptr<Enemy>> Enemies;
	std::vector<std::unique_ptr<DeathAnimation>> DeathAnimations;
	float SpawnInterval;
	float TimeSinceLastSpawn;
	float SpawnRadius;

private:

	void SpawnEnemy();

};