module DirectGL;

import :EllipseMode;

namespace DGL
{
	const EllipseMode& EllipseModeLTWH()
	{
		static EllipseMode mode = [](const float x1, const float y1, const float x2, const float y2) {
			return Math::FloatBoundary::FromLTWH(x1, y1, x2, y2);
		};

		return mode;
	}

	const EllipseMode& EllipseModeLTRB()
	{
		static EllipseMode mode = [](const float x1, const float y1, const float x2, const float y2) {
			
			return Math::FloatBoundary::FromLTRB(x1, y1, x2, y2);
		};

		return mode;
	}

	const EllipseMode& EllipseModeCenterRadius()
	{
		static EllipseMode mode = [](const float x1, const float y1, const float x2, const float y2) {
			return Math::FloatBoundary::FromLTWH(x1 - x2, y1 - y2, x2 * 2.0f, y2 * 2.0f);
		};

		return mode;
	}

	const EllipseMode& EllipseModeCenterDiameter()
	{
		static EllipseMode mode = [](const float x1, const float y1, const float x2, const float y2) {
			return Math::FloatBoundary::FromLTWH(x1 - x2 / 2.0f, y1 - y2 / 2.0f, x2, y2);
		};

		return mode;
	}
}