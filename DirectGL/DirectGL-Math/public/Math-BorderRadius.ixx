// Project Name : Math
// File Name    : Math-BorderRadius.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Math:BorderRadius;

export namespace DGL::Math
{
	struct BorderRadiusOnly
	{
		float TopLeft;
		float TopRight;
		float BottomRight;
		float BottomLeft;

		constexpr bool operator == (const BorderRadiusOnly&) const = default;
		constexpr bool operator != (const BorderRadiusOnly&) const = default;
	};

	class BorderRadius
	{
	public:

		float TopLeft;
		float TopRight;
		float BottomRight;
		float BottomLeft;

		constexpr BorderRadius();

		[[nodiscard]] static constexpr BorderRadius All(float radius);
		[[nodiscard]] static constexpr BorderRadius Only(const BorderRadiusOnly& only);

		constexpr bool operator == (const BorderRadius& other) const = default;
		constexpr bool operator != (const BorderRadius& other) const = default;

		static const BorderRadius Zero;

	private:

		constexpr explicit BorderRadius(float topLeft, float topRight, float bottomRight, float bottomLeft);

	};
}

namespace DGL::Math
{
	constexpr BorderRadius::BorderRadius() :
		TopLeft(0.0f),
		TopRight(0.0f),
		BottomRight(0.0f),
		BottomLeft(0.0f)
	{
	}

	constexpr BorderRadius BorderRadius::All(const float radius)
	{
		return Only({
			.TopLeft = radius,
			.TopRight = radius,
			.BottomRight = radius,
			.BottomLeft = radius
		});
	}

	constexpr BorderRadius BorderRadius::Only(const BorderRadiusOnly& only)
	{
		return BorderRadius{ only.TopLeft, only.TopRight, only.BottomRight, only.BottomLeft };
	}

	constexpr BorderRadius::BorderRadius(const float topLeft, const float topRight, const float bottomRight, const float bottomLeft) :
		TopLeft(topLeft), TopRight(topRight), BottomRight(bottomRight), BottomLeft(bottomLeft)
	{
	}

	inline constexpr BorderRadius BorderRadius::Zero = All(0.0f);
}