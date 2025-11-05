module;

#include <cstdint>
#include <concepts>
#include <cmath>
#include <algorithm>

export module DirectGL.Math:Value2;

import :Angle;
import :Random;

export namespace DGL::Math
{
	template <typename T>
	struct Value2
	{
		constexpr Value2();
		constexpr Value2(T x, T y);
		constexpr explicit Value2(T scalar);

		static constexpr Value2 FromAngle(Angle angle);
		static constexpr Value2 Random();

		template <std::convertible_to<T> U>
		constexpr explicit Value2(const Value2<U>& other)
			: X(static_cast<T>(other.X))
			, Y(static_cast<T>(other.Y))
		{
		}

		T Length() const;
		constexpr T LengthSquared() const;

		T Distance(const Value2& other) const;
		constexpr T DistanceSquared(const Value2& other) const;

		constexpr T Dot(const Value2& other) const;

		Angle Heading() const;
		Angle AngleBetween(const Value2& other) const;

		Value2 Limited(T maxLength) const;
		Value2 Normalized() const;
		Value2 Perpendicular() const;

		Value2 Rotated(Angle angle) const;

		constexpr Value2 operator - () const;
		constexpr Value2 operator + (const Value2& other) const;
		constexpr Value2 operator - (const Value2& other) const;
		constexpr Value2 operator / (const Value2& other) const;
		constexpr Value2 operator * (const Value2& other) const;

		Value2& operator += (const Value2& other);
		Value2& operator -= (const Value2& other);
		Value2& operator /= (const Value2& other);
		Value2& operator *= (const Value2& other);

		constexpr Value2 operator + (T value) const;
		constexpr Value2 operator - (T value) const;
		constexpr Value2 operator / (T value) const;
		constexpr Value2 operator * (T value) const;

		Value2& operator += (T value);
		Value2& operator -= (T value);
		Value2& operator /= (T value);
		Value2& operator *= (T value);

		constexpr bool operator == (const Value2& other) const = default;
		constexpr bool operator != (const Value2& other) const = default;

		static const Value2 Zero;
		static const Value2 One;

		T X;
		T Y;
	};

	template <typename T> constexpr Value2<T> operator + (T scalar, const Value2<T>& vector);
	template <typename T> constexpr Value2<T> operator - (T scalar, const Value2<T>& vector);
	template <typename T> constexpr Value2<T> operator / (T scalar, const Value2<T>& vector);
	template <typename T> constexpr Value2<T> operator * (T scalar, const Value2<T>& vector);

	typedef Value2<float> Float2;
	typedef Value2<int32_t> Int2;
	typedef Value2<uint32_t> Uint2;
}

namespace DGL::Math
{
	template <typename T>
	constexpr Value2<T>::Value2()
		: X(T{})
		, Y(T{})
	{
	}

	template <typename T>
	constexpr Value2<T>::Value2(const T x, const T y)
		: X(x)
		, Y(y)
	{
	}

	template <typename T>
	constexpr Value2<T>::Value2(const T scalar)
		: X(scalar)
		, Y(scalar)
	{
	}

	template <typename T> constexpr Value2<T> Value2<T>::FromAngle(const Angle angle)
	{
		const T radians = static_cast<T>(angle.AsRadians());
		return Value2{ std::cos(radians), std::sin(radians) };
	}

	template <typename T> constexpr Value2<T> Value2<T>::Random() { return FromAngle(Degrees(Math::Random(360.0f))); }

	template <typename T> T Value2<T>::Length() const { return static_cast<T>(std::sqrt(LengthSquared())); }
	template <typename T> constexpr T Value2<T>::LengthSquared() const { return X * X + Y * Y; }

	template <typename T> T Value2<T>::Distance(const Value2& other) const { return (*this - other).Length(); }
	template <typename T> constexpr T Value2<T>::DistanceSquared(const Value2& other) const { return (*this - other).LengthSquared(); }

	template <typename T> constexpr T Value2<T>::Dot(const Value2& other) const { return X * other.X + Y * other.Y; }

	template <typename T> Angle Value2<T>::Heading() const { return Radians(std::atan2(Y, X)); }
	template <typename T> Angle Value2<T>::AngleBetween(const Value2& other) const
	{
		const T dotProduct = Dot(other);
		const T lengthsProduct = Length() * other.Length();
		if (lengthsProduct == T{})
		{
			return Angle::Zero;
		}

		const T cosineOfAngle = dotProduct / lengthsProduct;
		const T clampedCosine = std::clamp(cosineOfAngle, static_cast<T>(-1), static_cast<T>(1));
		return Radians(static_cast<float>(std::acos(clampedCosine)));
	}

	template <typename T> Value2<T> Value2<T>::Limited(const T maxLength) const
	{
		const T length = LengthSquared();
		if (length > (maxLength * maxLength))
		{
			return Normalized() * maxLength;
		}

		return *this;
	}

	template <typename T> Value2<T> Value2<T>::Normalized() const
	{
		const T length = Length();
		if (length == T{})
		{
			return Value2{};
		}

		return Value2{ X / length, Y / length };
	}

	template <typename T> Value2<T> Value2<T>::Perpendicular() const { return Value2{ -Y, X }; }
	template <typename T> Value2<T> Value2<T>::Rotated(const Angle angle) const
	{
		const T radians = static_cast<T>(angle.AsRadians());
		const T cosAngle = std::cos(radians);
		const T sinAngle = std::sin(radians);

		return Value2 {
			X * cosAngle - Y * sinAngle,
			X * sinAngle + Y * cosAngle
		};
	}

	template <typename T> constexpr Value2<T> Value2<T>::operator-() const { return { -X, -Y }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator+(const Value2& other) const { return { X + other.X, Y + other.Y }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator-(const Value2& other) const { return { X - other.X, Y - other.Y }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator/(const Value2& other) const { return { X / other.X, Y / other.Y }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator*(const Value2& other) const { return { X * other.X, Y * other.Y }; }

	template <typename T> Value2<T>& Value2<T>::operator+=(const Value2& other) { X += other.X; Y += other.Y; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator-=(const Value2& other) { X -= other.X; Y -= other.Y; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator/=(const Value2& other) { X /= other.X; Y /= other.Y; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator*=(const Value2& other) { X *= other.X; Y *= other.Y; return *this; }

	template <typename T> constexpr Value2<T> Value2<T>::operator+(T value) const { return { X + value, Y + value }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator-(T value) const { return { X - value, Y - value }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator/(T value) const { return { X / value, Y / value }; }
	template <typename T> constexpr Value2<T> Value2<T>::operator*(T value) const { return { X * value, Y * value }; }

	template <typename T> Value2<T>& Value2<T>::operator+=(const T value) { X += value; Y += value; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator-=(const T value) { X -= value; Y -= value; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator/=(const T value) { X /= value; Y /= value; return *this; }
	template <typename T> Value2<T>& Value2<T>::operator*=(const T value) { X *= value; Y *= value; return *this; }

	template <typename T> constexpr Value2<T> operator+(T scalar, const Value2<T>& vector) { return Value2<T>{ scalar + vector.X, scalar + vector.Y }; }
	template <typename T> constexpr Value2<T> operator-(T scalar, const Value2<T>& vector) { return Value2<T>{ scalar - vector.X, scalar - vector.Y }; }
	template <typename T> constexpr Value2<T> operator/(T scalar, const Value2<T>& vector) { return Value2<T>{ scalar / vector.X, scalar / vector.Y }; }
	template <typename T> constexpr Value2<T> operator*(T scalar, const Value2<T>& vector) { return Value2<T>{ scalar* vector.X, scalar* vector.Y }; }

	template <typename T> inline constexpr Value2<T> Value2<T>::Zero = Value2{ T{}, T{} };
	template <typename T> inline constexpr Value2<T> Value2<T>::One = Value2{ T{1}, T{1} };
	
}