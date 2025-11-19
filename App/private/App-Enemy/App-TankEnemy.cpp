module;

#include <utility>
#include <memory>

module App;

import :TankEnemy;
import :ParticleDeathAnimation;

TankEnemy::TankEnemy(const DGL::Math::Float2 position) :
	Enemy {
		position,
		DGL::Math::Float2{ 40.0f, 40.0f },
		5,
		80.0f,
		30.0f,
	},
	SpawnAnimationProgress{ 0.0f },
	HealthFlashProgress{ 0.0f }
{
}

void TankEnemy::Seek(const DGL::Math::Float2 targetPosition)
{
	const DGL::Math::Float2 desired = (targetPosition - Position).Normalized() * MaxSpeed;
	const DGL::Math::Float2 steer = (desired - Velocity).Limited(MaxForce);
	ApplyForce(steer);
}

void TankEnemy::Update(const float deltaTime)
{
	SpawnAnimationProgress = std::min(SpawnAnimationProgress + deltaTime / 2.0f, 1.0f);
	HealthFlashProgress = std::max(HealthFlashProgress - deltaTime * 5.0f, 0.0f);

	UpdatePendingDamages(deltaTime);
	Move(deltaTime);
}

void TankEnemy::Show() const
{
	const float scale = DGL::Math::Easings::EaseOutElastic(SpawnAnimationProgress);
	const float alpha = HealthFlashProgress;
	DGL::PushStyle();
	{
		const DGL::Math::BorderRadius radius = DGL::Math::BorderRadius::All(5.0f);

		DGL::Translate(Position.X, Position.Y);
		DGL::Scale(scale, scale);
		DGL::SetFillColorDisabled();
		DGL::SetRectMode(DGL::RectModeCenterWH());

		// Draw red rectangle as "background layer"
		DGL::SetStrokeWeight(10.0f);
		DGL::SetFillColor({ COLOR_GOLDEN, alpha });
		DGL::SetStrokeColor({ COLOR_GOLDEN, 1.0f });
		DGL::RoundedRect(0.0f, 0.0f, Size.X, Size.Y, radius);

		// Draw white rectangle as "foreground layer"
		DGL::SetStrokeWeight(5.0f);
		DGL::SetStrokeColor({ COLOR_WHITE, 1.0f });
		DGL::RoundedRect(0.0f, 0.0f, Size.X, Size.Y, radius);
	}
	DGL::PopStyle();
}

std::unique_ptr<DeathAnimation> TankEnemy::CreateDeathAnimation() const
{
	return std::make_unique<ParticleDeathAnimation>(Position, 15, COLOR_GOLDEN);
}

void TankEnemy::OnDamage(int amount)
{
	HealthFlashProgress = 1.0f;
}