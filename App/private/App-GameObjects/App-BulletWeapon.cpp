module;

#include <type_traits>
#include <ranges>
#include <algorithm>

module App;

import :BulletWeapon;

using namespace DGL;

inline static constexpr float BULLET_SPEED = 750.0f;

enum class ShootMode
{
	Single,
	Multishot,
};

ShootMode GetShootMode()
{
	return ShootMode::Single;
}

BulletWeapon::BulletWeapon(const PositionProvider& emissionPosition, const float shootInterval, const float shootRadius):
	EmissionPosition{ &emissionPosition },
	ShootInterval{ shootInterval },
	TimeElapsedSinceLastShot{ 0.0f },
	ShootRadius{ shootRadius },
	SpawnAnimationProgress{ 0.0f }
{
}

void BulletWeapon::Shoot(std::vector<std::unique_ptr<Enemy>>& enemies)
{
	// If we are not ready to shoot yet, return early
	if (TimeElapsedSinceLastShot < ShootInterval)
	{
		return; // Cannot shoot yet
	}

	const auto& position = EmissionPosition->GetPosition();
	const std::vector<SortedEnemy> sortedEnemies = GetSortedEnemies(enemies);

	const size_t targetCount = []
	{
		switch (GetShootMode())
		{
			case ShootMode::Single: return 1;
			case ShootMode::Multishot: return 2;
			default: return 1;
		};
	}();

	const auto targets = sortedEnemies | std::ranges::views::take(targetCount); //!< Get the first N targets to shoot at

	// Iterate over the list of sorted enemies and shoot at the first one in sight
	for (const SortedEnemy& enemy : targets)
	{
		// Compute the distance from the weapon to the enemy's border pointing towards the weapon
		const float distanceToEnemyBorder = enemy.Distance - enemy.Enemy->Size.Length() / 2.0f;

		// Compute the bullet lifetime it will take to reach the enemy
		// Note that we consider the enemy's velocity here to lead the shot correctly
		const float bulletLifeTime = distanceToEnemyBorder / (BULLET_SPEED + enemy.Enemy->Velocity.Length());

		// Compute the direction & velocity of the bullet
		const Math::Float2 direction = enemy.Difference / enemy.Distance;
		const Math::Float2 bulletVelocity = direction * BULLET_SPEED;

		Bullets.push_back(Bullet { 
			.Position = position,
			.Velocity = bulletVelocity,
			.LifeTime = bulletLifeTime,
			.TimeAlive = 0.0f
		});

		enemy.Enemy->ApplyDamage(DelayedDamage {
			.Amount = 1,
			.Delay = bulletLifeTime
		});
	}

	TimeElapsedSinceLastShot = 0.0f;
}

void BulletWeapon::Update(const float deltaTime)
{
	TimeElapsedSinceLastShot += deltaTime;
	SpawnAnimationProgress = std::min(SpawnAnimationProgress + deltaTime, 1.0f);

	// Update all bullets and remove the ones that have exceeded their lifetime
	for (int i = Bullets.size() - 1; i >= 0; --i)
	{
		Bullet& bullet = Bullets[i];
		bullet.Position += bullet.Velocity * deltaTime;
		bullet.TimeAlive += deltaTime;

		if (bullet.TimeAlive >= bullet.LifeTime)
		{
			Bullets.erase(Bullets.begin() + i);
		}
	}
}

void BulletWeapon::Show() const
{
	// Render the shoot radius
	PushStyle();
	{
		const auto& [px, py] = EmissionPosition->GetPosition();
		const float scale = Math::Easings::EaseOutBack(SpawnAnimationProgress);
		Translate(px, py);
		Scale(scale, scale);
		SetFillColorDisabled();
		SetStrokeColor({ COLOR_BLUE, 1.0f });
		SetStrokeWeight(2.0f);
		SetEllipseMode(EllipseModeCenterRadius());
		Circle(0.0f, 0.0f, ShootRadius);
	}
	PopStyle();

	// Render all bullets
	PushStyle();
	{
		SetFillColor({ COLOR_WHITE, 1.0f });
		SetStrokeColor({ COLOR_RED, 1.0f });
		SetStrokeWeight(3.0f);
		SetEllipseMode(EllipseModeCenterRadius());
		for (const Bullet& bullet : Bullets)
		{
			Ellipse(bullet.Position.X, bullet.Position.Y, 5.0f, 5.0f);
		}
	}
	PopStyle();
}

std::vector<BulletWeapon::SortedEnemy> BulletWeapon::GetSortedEnemies(std::vector<std::unique_ptr<Enemy>>& enemies) const
{
	const Math::Float2& position = EmissionPosition->GetPosition();

	// Create a list of enemies that we are allowed to shoot at.
	std::vector<SortedEnemy> sortedEnemies = enemies
		| std::ranges::views::filter([](const std::unique_ptr<Enemy>& enemy) { return not enemy->IsAboutToDie(); })
		| std::ranges::views::transform([position](const std::unique_ptr<Enemy>& enemy)
		{
			const Math::Float2 difference = enemy->Position - position;

			return SortedEnemy {
				.Enemy = enemy.get(),
				.Difference = difference,
				.Distance = difference.Length(),
				.DistanceSquared = difference.LengthSquared()
			};
		})
		| std::ranges::views::filter([this](const SortedEnemy& sortedEnemy) { return sortedEnemy.Distance < ShootRadius; })
		| std::ranges::to<std::vector>();

	// Sort the enemies by the time they would reach the weapon
	std::ranges::sort(sortedEnemies, [this](const SortedEnemy& a, const SortedEnemy& b)
	{
		const float distanceA = a.DistanceSquared / a.Enemy->Velocity.LengthSquared();
		const float distanceB = b.DistanceSquared / b.Enemy->Velocity.LengthSquared();
		return distanceA < distanceB;
	});

	return sortedEnemies;
}