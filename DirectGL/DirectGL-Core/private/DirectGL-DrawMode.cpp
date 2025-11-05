module DirectGL;

import :RectMode;

namespace DGL
{
	const RectMode& RectModeLTWH()
	{
		static RectMode mode = [](const float x, const float y, const float width, const float height) -> Math::FloatBoundary
		{
			return Math::FloatBoundary::FromLTWH(x, y, width, height);
		};

		return mode;
	}

	const RectMode& RectModeLTRB()
	{
		static RectMode mode = [](const float left, const float top, const float right, const float bottom) -> Math::FloatBoundary
		{
			return Math::FloatBoundary::FromLTRB(left, top, right, bottom);
		};
		return mode;
	}

	const RectMode& RectModeCenterWH()
	{
		static RectMode mode = [](const float centerX, const float centerY, const float width, const float height) -> Math::FloatBoundary
		{
			const float left = centerX - (width / 2.0f);
			const float top = centerY - (height / 2.0f);
			return Math::FloatBoundary::FromLTWH(left, top, width, height);
		};

		return mode;
	}
}