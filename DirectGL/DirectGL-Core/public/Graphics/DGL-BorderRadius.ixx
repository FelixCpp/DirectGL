// Project Name : DirectGL
// File Name    : DGL-BorderRadius.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:BorderRadius;

import :Radius;

export namespace DGL
{
	struct BorderRadiusOnly
	{
		Radius TopLeft;
		Radius TopRight;
		Radius BottomRight;
		Radius BottomLeft;
	};

	class BorderRadius
	{
	public:

		Radius TopLeft;
		Radius TopRight;
		Radius BottomRight;
		Radius BottomLeft;

		[[nodiscard]] static constexpr BorderRadius All(Radius radius);
		[[nodiscard]] static constexpr BorderRadius Symmetric(float horizontal, float vertical);
		[[nodiscard]] static constexpr BorderRadius Only(const BorderRadiusOnly& only);

	private:

		constexpr explicit BorderRadius(Radius topLeft, Radius topRight, Radius bottomRight, Radius bottomLeft);

	};
}

namespace DGL
{
	constexpr BorderRadius BorderRadius::All(const Radius radius)
	{
		return Only({
			.TopLeft = radius,
			.TopRight = radius,
			.BottomRight = radius,
			.BottomLeft = radius
		});
	}

	constexpr BorderRadius BorderRadius::Symmetric(const float horizontal, const float vertical)
	{
		const Radius cornerRadius = Radius::Elliptical(horizontal, vertical);

		return Only({
			.TopLeft = cornerRadius,
			.TopRight = cornerRadius,
			.BottomRight = cornerRadius,
			.BottomLeft = cornerRadius,
		});
	}

	constexpr BorderRadius BorderRadius::Only(const BorderRadiusOnly& only)
	{
		return BorderRadius{ only.TopLeft, only.TopRight, only.BottomRight, only.BottomLeft };
	}

	constexpr BorderRadius::BorderRadius(const Radius topLeft, const Radius topRight, const Radius bottomRight, const Radius bottomLeft) :
		TopLeft(topLeft), TopRight(topRight), BottomRight(bottomRight), BottomLeft(bottomLeft)
	{
	}
}