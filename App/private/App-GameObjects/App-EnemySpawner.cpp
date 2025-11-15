module;

#include <memory>

module App;

import :EnemySpawner;
import :SliceDeathAnimation;
import :ParticleDeathAnimation;
import :NormalEnemy;
import :TankEnemy;

EnemySpawner::EnemySpawner(
	const PositionProvider& positionProvider,
	const float spawnInterval,
	const float spawnRadius
) :	SpawnCenter{ &positionProvider },
	SpawnInterval{ spawnInterval },
	TimeSinceLastSpawn{ 0.0f },
	SpawnRadius{ spawnRadius }
{
}

void EnemySpawner::Update(const float deltaTime)
{
	// Spawn new enemies at regular intervals
	TimeSinceLastSpawn += deltaTime;
	while (TimeSinceLastSpawn >= SpawnInterval)
	{
		SpawnEnemy();
	}

	// Update all enemies and remove the dead ones.
	for (size_t i = Enemies.size(); i-- > 0;)
	{
		Enemy& enemy = *Enemies[i];
		enemy.Seek(SpawnCenter->GetPosition());
		enemy.Update(deltaTime);

		if (enemy.IsDead())
		{
			DeathAnimations.push_back(enemy.CreateDeathAnimation());
			Enemies.erase(Enemies.begin() + i);
		}
	}
	
	// Update all death animations and remove the finished ones
	for (size_t i = DeathAnimations.size(); i-- > 0;)
	{
		DeathAnimation& animation = *DeathAnimations[i];
		animation.Update(deltaTime);

		if (animation.IsFinished())
		{
			DeathAnimations.erase(DeathAnimations.begin() + i);
		}
	}
}

void EnemySpawner::Show() const
{
	// Render all enemies
	for (const std::unique_ptr<Enemy>& enemy : Enemies)
	{
		enemy->Show();
	}

	// Render all death animations
	for (const std::unique_ptr<DeathAnimation>& animation : DeathAnimations)
	{
		animation->Show();
	}
}

void EnemySpawner::SpawnEnemy()
{
	// Spawn enemy at a random position around the spawn center within the spawn radius
	const DGL::Math::Float2 randomDirection = DGL::Math::Float2::Random();
	const DGL::Math::Float2 spawnOffset = randomDirection * (SpawnRadius + DGL::Math::Random(500.0f));
	const DGL::Math::Float2 spawnPosition = SpawnCenter->GetPosition() + spawnOffset;

	if (DGL::Math::Random(1.0f) < 0.9f)
	{
		Enemies.push_back(std::make_unique<NormalEnemy>(spawnPosition));
	}
	else
	{
		Enemies.push_back(std::make_unique<TankEnemy>(spawnPosition));
	}

	TimeSinceLastSpawn -= SpawnInterval;
}