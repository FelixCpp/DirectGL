module;

#include <cmath>

module DirectGL;

import :Math;

namespace DGL
{
	const RectMode& RectModeLTWH()
	{
		static EllipseMode mode = [](const float x, const float y, const float width, const float height) {
			return FloatBoundary::FromLTWH(x, y, width, height);
		};

		return mode;
	}

	const RectMode& RectModeLTRB()
	{
		static EllipseMode mode = [](const float left, const float top, const float right, const float bottom) {
			return FloatBoundary::FromLTRB(left, top, right, bottom);
		};

		return mode;
	}

	const RectMode& RectModeCenterWH()
	{
		static EllipseMode mode = [](const float centerX, const float centerY, const float width, const float height) {
			const float halfWidth = width * 0.5f;
			const float halfHeight = height * 0.5f;
			return FloatBoundary::FromLTRB(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);
		};

		return mode;
	}
}

namespace DGL
{
	const EllipseMode& EllipseModeLTRB()
	{
		static EllipseMode mode = [](const float left, const float top, const float right, const float bottom) {
			return FloatBoundary::FromLTRB(left, top, right, bottom);
		};

		return mode;
	}

	const EllipseMode& EllipseModeLTWH()
	{
		static EllipseMode mode = [](const float x, const float y, const float width, const float height) {
			return FloatBoundary::FromLTWH(x, y, width, height);
		};

		return mode;
	}

	const EllipseMode& EllipseModeCenterWH()
	{
		static EllipseMode mode = [](const float centerX, const float centerY, const float width, const float height) {
			const float halfWidth = width * 0.5f;
			const float halfHeight = height * 0.5f;
			return FloatBoundary::FromLTRB(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);
		};

		return mode;
	}

	const EllipseMode& EllipseModeCenterRadius()
	{
		static EllipseMode mode = [](const float centerX, const float centerY, const float radiusX, const float radiusY) {
			return FloatBoundary::FromLTRB(centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
		};

		return mode;
	}

	const EllipseMode& EllipseModeCenterDiameter()
	{
		static EllipseMode mode = [](const float centerX, const float centerY, const float diameterX, const float diameterY) {
			const float radiusX = diameterX * 0.5f;
			const float radiusY = diameterY * 0.5f;
			return FloatBoundary::FromLTRB(centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
		};

		return mode;
	}
}

namespace DGL
{
	const SegmentCountMode& SegmentCountModeFixed(const size_t count)
	{
		static SegmentCountMode mode = [count](const Radius) {
			return count;
		};

		return mode;
	}

	const SegmentCountMode& SegmentCountModeSmooth(const float error)
	{
		static SegmentCountMode mode = [error](const Radius radius) -> size_t {
			constexpr float MAX_POINT_ACCURACY = 200.0f;
			constexpr float MIN_POINT_ACCURACY = 20.0f;
			
			const float targetRadius = radius.Max();
			if (error <= 0.0f or targetRadius <= 0.0f) return 0;

			const float angle = std::acosf(1.0f - Constrain(error / targetRadius, 0.0f, 1.0f));
			if (angle <= 0.0f) return 0;
			const float segments = std::ceilf(PI / angle);

			return Constrain(segments, MIN_POINT_ACCURACY, MAX_POINT_ACCURACY);
		};

		return mode;
	}
}