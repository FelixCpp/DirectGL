// Project Name : Math
// File Name    : Math-BorderRadius.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Math:BorderRadius;

import :Radius;

export namespace DGL::Math
{
	struct BorderRadiusOnly
	{
		Radius TopLeft;
		Radius TopRight;
		Radius BottomRight;
		Radius BottomLeft;

		constexpr bool operator == (const BorderRadiusOnly&) const = default;
		constexpr bool operator != (const BorderRadiusOnly&) const = default;
	};

	class BorderRadius
	{
	public:

		Radius TopLeft;
		Radius TopRight;
		Radius BottomRight;
		Radius BottomLeft;

		constexpr BorderRadius();

		[[nodiscard]] static constexpr BorderRadius All(Radius radius);
		[[nodiscard]] static constexpr BorderRadius Symmetric(float horizontal, float vertical);
		[[nodiscard]] static constexpr BorderRadius Only(const BorderRadiusOnly& only);

		constexpr bool operator == (const BorderRadius& other) const = default;
		constexpr bool operator != (const BorderRadius& other) const = default;

		static const BorderRadius Zero;

	private:

		constexpr explicit BorderRadius(Radius topLeft, Radius topRight, Radius bottomRight, Radius bottomLeft);

	};
}

namespace DGL::Math
{
	constexpr BorderRadius::BorderRadius() :
		TopLeft(Radius::Zero),
		TopRight(Radius::Zero),
		BottomRight(Radius::Zero),
		BottomLeft(Radius::Zero)
	{
	}

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

	inline constexpr BorderRadius BorderRadius::Zero = All(Radius::Zero);
}