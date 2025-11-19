module;

#include <utility>

module App;

import :Tower;

using namespace DGL;

Tower::Tower(const struct PositionProvider& positionProvider):
	PositionProvider(&positionProvider),
	SpawnAnimationProgress(0.0f)
{
}

void Tower::Update(const float deltaTime)
{
	SpawnAnimationProgress = std::min(SpawnAnimationProgress + deltaTime, 1.0f);
}

void Tower::Show() const
{
	const float scale = Math::Easings::EaseOutBack(SpawnAnimationProgress);
	const auto [px, py] = PositionProvider->GetPosition();

	// Render the tower here
	PushStyle();
	{
		Translate(px, py);
		Scale(scale, scale);
		//SetEllipseSegmentsMode(EllipseSegmentsModeFixed(6));
		SetEllipseMode(EllipseModeCenterRadius());
		SetFillColorDisabled();
	
		// Draw blue hexagon as "background layer"
		SetStrokeWeight(8.0f);
		SetStrokeColor({ COLOR_BLUE, 1.0f });
		Circle(0.0f, 0.0f, 50.0f);
	
		// Draw white hexagon as "foreground layer"
		SetStrokeWeight(4.0f);
		SetStrokeColor({ COLOR_WHITE, 1.0f });
		Circle(0.0f, 0.0f, 50.0f);
	}
	PopStyle();
}