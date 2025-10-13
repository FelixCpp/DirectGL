module DirectGL;

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
