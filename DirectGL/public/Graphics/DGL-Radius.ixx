// Project Name : DirectGL
// File Name    : DGL-Radius.ixx
// Author       : Felix Busch
// Created Date : 2025/09/26

export module DGL:Radius;

export namespace DGL
{
	class Radius
	{
	public:

		float X;
		float Y;

		static const Radius Zero;

		[[nodiscard]] static constexpr Radius Circular(float radius);
		[[nodiscard]] static constexpr Radius Elliptical(float radiusX, float radiusY);

	private:

		constexpr explicit Radius(float radiusX, float radiusY);

	};

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
	constexpr Radius Radius::Circular(const float radius)
	{
		return Radius{ radius, radius };
	}

	constexpr Radius Radius::Elliptical(const float radiusX, const float radiusY)
	{
		return Radius{ radiusX, radiusY };
	}

	constexpr Radius::Radius(const float radiusX, const float radiusY):
		X(radiusX), Y(radiusY)
	{
	}

	inline constexpr Radius Radius::Zero = Circular(0.0f);
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
		return BorderRadius { only.TopLeft, only.TopRight, only.BottomRight, only.BottomLeft };
	}

	constexpr BorderRadius::BorderRadius(const Radius topLeft, const Radius topRight, const Radius bottomRight, const Radius bottomLeft):
		TopLeft(topLeft), TopRight(topRight), BottomRight(bottomRight), BottomLeft(bottomLeft)
	{
	}
}