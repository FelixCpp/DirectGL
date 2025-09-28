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