module App;

import :SliceDeathAnimation;

using namespace DGL;

SliceDeathAnimation::SliceDeathAnimation(const Math::Float2 position, const Math::Float2 size, const float duration):
	ElapsedTime(0.0f),
	Duration(duration)
{
	// Generate 4 slices going in different directions
	const std::array directions = {
		Math::Float2::FromAngle(Math::Degrees(5.0f * 45.0f)),
		Math::Float2::FromAngle(Math::Degrees(7.0f * 45.0f)),
		Math::Float2::FromAngle(Math::Degrees(1.0f * 45.0f)),
		Math::Float2::FromAngle(Math::Degrees(3.0f * 45.0f)),
	};

	const std::array offsets = {
		Math::Float2(-size.X / 4.0f, -size.Y / 4.0f),
		Math::Float2( size.X / 4.0f, -size.Y / 4.0f),
		Math::Float2( size.X / 4.0f,  size.Y / 4.0f),
		Math::Float2(-size.X / 4.0f,  size.Y / 4.0f),
	};

	for (size_t i = 0; i < Slices.size(); ++i)
	{
		Slices[i] = Slice {
			.Position = position + offsets[i],
			.Velocity = directions[i] * Math::Float2::Random() * 10.0f,
			.Size = size / 4.0f,
			.LifeTime = duration,
			.TimeAlive = 0.0f
		};
	}
}

bool SliceDeathAnimation::IsFinished() const
{
	return ElapsedTime >= Duration;
}

void SliceDeathAnimation::Update(const float deltaTime)
{
	for (Slice& slice : Slices)
	{
		slice.TimeAlive += deltaTime;
		slice.Position += slice.Velocity * deltaTime;
	}
}

void SliceDeathAnimation::Show() const
{
	for (const Slice& slice : Slices)
	{
		const float progress = slice.TimeAlive / slice.LifeTime;
		if (progress >= 1.0f)
			continue;

		const float alpha = 1.0f - progress;

		PushStyle();
		{
			Translate(slice.Position.X, slice.Position.Y);
			RectModeCenterWH();

			// Draw red rectangle as "background layer"
			SetStrokeWeight(4.0f);
			SetFillColor({ COLOR_RED, alpha });
			SetStrokeColor({ COLOR_RED, alpha });
			Rect(0.0f, 0.0f, slice.Size.X, slice.Size.Y);

			// Draw white rectangle as "foreground layer"
			SetFillColorDisabled();
			SetStrokeWeight(2.0f);
			SetStrokeColor({ COLOR_WHITE, alpha });
			Rect(0.0f, 0.0f, slice.Size.X, slice.Size.Y);

		}
		PopStyle();
	}
}