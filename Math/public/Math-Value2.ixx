module;

#include <cstdint>

export module Math:Value2;

namespace Math
{
	export template <typename T>
	struct Value2
	{
		constexpr Value2();
		constexpr Value2(T x, T y);
		constexpr explicit Value2(T scalar);

		constexpr bool operator == (const Value2& other) const;
		constexpr bool operator != (const Value2& other) const;

		T X;
		T Y;
	};

	export typedef Value2<float> Float2;
	export typedef Value2<int32_t> Int2;
	export typedef Value2<uint32_t> Uint2;
}

namespace Math
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

	template <typename T>
	constexpr bool Value2<T>::operator==(const Value2<T>& other) const
	{
		return X == other.X and Y == other.Y;
	}

	template <typename T>
	constexpr bool Value2<T>::operator!=(const Value2& other) const
	{
		return X != other.X or Y != other.Y;
	}
}