module DirectGL;

namespace DGL
{
	RectMode RectModeLTWH()
	{
		return [](const float x, const float y, const float width, const float height) {
			return FloatBoundary::FromLTWH(x, y, width, height);
		};
	}

	RectMode RectModeLTRB()
	{
		return [](const float left, const float top, const float right, const float bottom) {
			return FloatBoundary::FromLTRB(left, top, right, bottom);
		};
	}

	RectMode RectModeCenterWH()
	{
		return [](const float centerX, const float centerY, const float width, const float height) {
			const float halfWidth = width * 0.5f;
			const float halfHeight = height * 0.5f;
			return FloatBoundary::FromLTRB(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);
		};
	}
}

namespace DGL
{
	EllipseMode EllipseModeLTRB()
	{
		return [](const float left, const float top, const float right, const float bottom) {
			return FloatBoundary::FromLTRB(left, top, right, bottom);
			};
	}

	EllipseMode EllipseModeLTWH()
	{
		return [](const float x, const float y, const float width, const float height) {
			return FloatBoundary::FromLTWH(x, y, width, height);
		};
	}

	EllipseMode EllipseModeCenterWH()
	{
		return [](const float centerX, const float centerY, const float width, const float height) {
			const float halfWidth = width * 0.5f;
			const float halfHeight = height * 0.5f;
			return FloatBoundary::FromLTRB(centerX - halfWidth, centerY - halfHeight, centerX + halfWidth, centerY + halfHeight);
		};
	}

	EllipseMode EllipseModeCenterRadius()
	{
		return [](const float centerX, const float centerY, const float radiusX, const float radiusY) {
			return FloatBoundary::FromLTRB(centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
		};
	}

	EllipseMode EllipseModeCenterDiameter()
	{
		return [](const float centerX, const float centerY, const float diameterX, const float diameterY) {
			const float radiusX = diameterX * 0.5f;
			const float radiusY = diameterY * 0.5f;
			return FloatBoundary::FromLTRB(centerX - radiusX, centerY - radiusY, centerX + radiusX, centerY + radiusY);
		};
	}
}
