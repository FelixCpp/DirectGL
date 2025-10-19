// Project Name : DirectGL-Math
// File Name    : Math-Angle.ixx
// Author       : Felix Busch
// Created Date : 2025/10/12

export module DirectGL.Math:Angle;

import :Constants;

export namespace DGL::Math
{
	class Angle
	{
	public:

		[[nodiscard]] constexpr float AsDegrees() const;
		[[nodiscard]] constexpr float AsRadians() const;

		[[nodiscard]] constexpr bool operator == (Angle other) const;
		[[nodiscard]] constexpr bool operator != (Angle other) const;

		constexpr Angle operator + (Angle other) const;
		constexpr Angle operator - (Angle other) const;
		constexpr Angle operator * (float scalar) const;
		constexpr Angle operator / (float scalar) const;

		static const Angle Zero;

	private:

		constexpr explicit Angle(float degrees);

		float m_Degrees;

		friend constexpr Angle Degrees(float degrees);
		friend constexpr Angle Radians(float radians);

	};

	[[nodiscard]] constexpr Angle Degrees(float degrees);
	[[nodiscard]] constexpr Angle Radians(float radians);
}

namespace DGL::Math
{
	inline static constexpr float DEG_TO_RAD = PI / 180.0f;
	inline static constexpr float RAD_TO_DEG = 180.0f / PI;

	constexpr float Angle::AsDegrees() const { return m_Degrees; }
	constexpr float Angle::AsRadians() const { return m_Degrees * DEG_TO_RAD; }

	constexpr bool Angle::operator==(const Angle other) const { return m_Degrees == other.m_Degrees; }
	constexpr bool Angle::operator!=(const Angle other) const { return m_Degrees != other.m_Degrees; }

	constexpr Angle Angle::operator+(const Angle other) const { return Angle{ m_Degrees + other.m_Degrees }; }
	constexpr Angle Angle::operator-(const Angle other) const { return Angle{ m_Degrees - other.m_Degrees }; }
	constexpr Angle Angle::operator*(const float scalar) const { return Angle{ m_Degrees * scalar }; }
	constexpr Angle Angle::operator/(const float scalar) const { return Angle{ m_Degrees / scalar }; }

	constexpr Angle::Angle(const float degrees): m_Degrees(degrees) { }

	constexpr Angle Degrees(const float degrees) { return Angle{ degrees }; }
	constexpr Angle Radians(const float radians) { return Angle{ radians * RAD_TO_DEG }; }

	inline constexpr Angle Angle::Zero = Degrees(0.0f);
}
