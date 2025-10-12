// Project Name : Math
// File Name    : Math-Radius.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Math:Radius;

export namespace DGL::Math
{
	class Radius
	{
	public:

		float X;
		float Y;

		static const Radius Zero;

		[[nodiscard]] static constexpr Radius Circular(float radius);
		[[nodiscard]] static constexpr Radius Elliptical(float radiusX, float radiusY);

		constexpr bool operator == (Radius other) const;
		constexpr bool operator != (Radius other) const;

	private:

		constexpr explicit Radius(float radiusX, float radiusY);

	};
}

namespace DGL::Math
{
	constexpr Radius Radius::Circular(const float radius)
	{
		return Radius{ radius, radius };
	}

	constexpr Radius Radius::Elliptical(const float radiusX, const float radiusY)
	{
		return Radius{ radiusX, radiusY };
	}

	constexpr Radius::Radius(const float radiusX, const float radiusY) :
		X(radiusX), Y(radiusY)
	{
	}

	constexpr bool Radius::operator==(const Radius other) const
	{
		return X == other.X && Y == other.Y;
	}

	constexpr bool Radius::operator!=(const Radius other) const
	{
		return !(*this == other);
	}

	inline constexpr Radius Radius::Zero = Circular(0.0f);
}