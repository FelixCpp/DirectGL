module;

#include <utility>
#include <memory>

module App;

import :NormalEnemy;
import :ParticleDeathAnimation;

using namespace DGL;

NormalEnemy::NormalEnemy(const Math::Float2 position) :
	Enemy {
		position,
		Math::Float2{20.0f, 20.0f},
		1,
		100.0f,
		50.0f,
	},
	SpawnAnimationProgress{0.0f},
	HealthFlashProgress{0.0f}
{
}

void NormalEnemy::Update(const float deltaTime)
{
	SpawnAnimationProgress = std::min(SpawnAnimationProgress + deltaTime / 2.0f, 1.0f);
	HealthFlashProgress = std::max(HealthFlashProgress - deltaTime * 5.0f, 0.0f);

	UpdatePendingDamages(deltaTime);
	Move(deltaTime);
}

void NormalEnemy::Seek(const Math::Float2 target)
{
	const Math::Float2 desired = (target - Position).Normalized() * MaxSpeed;
	const Math::Float2 steer = (desired - Velocity).Limited(MaxForce);
	ApplyForce(steer);
}

void NormalEnemy::Show() const
{
	const float scale = Math::Easings::EaseOutElastic(SpawnAnimationProgress);
	const float alpha = HealthFlashProgress;

	PushStyle();
	{
		Translate(Position.X, Position.Y);
		Scale(scale, scale);
		SetFillColorDisabled();
		SetRectMode(RectModeCenterWH());

		// Draw red rectangle as "background layer"
		SetStrokeWeight(8.0f);
		SetFillColor({ COLOR_RED, alpha });
		SetStrokeColor({ COLOR_RED, 1.0f });
		Rect(0.0f, 0.0f, Size.X, Size.Y);

		// Draw white rectangle as "foreground layer"
		SetStrokeWeight(2.0f);
		SetStrokeColor({ COLOR_WHITE, 1.0f });
		Rect(0.0f, 0.0f, Size.X, Size.Y);
	}
	
	PopStyle();
}

std::unique_ptr<DeathAnimation> NormalEnemy::CreateDeathAnimation() const
{
	return std::make_unique<ParticleDeathAnimation>(Position, 10, COLOR_RED);
}

void NormalEnemy::OnDamage(int amount)
{
	HealthFlashProgress = 1.0f;
}
