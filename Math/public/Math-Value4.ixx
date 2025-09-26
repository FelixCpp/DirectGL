// Project Name : Math
// File Name    : Math-Value4.ixx
// Author       : Felix Busch
// Created Date : 2025/09/23

module;

#include <cstdint>

export module Math:Value4;

import :Value2;

export namespace Math
{
	template <typename T>
	struct Value4
	{
		constexpr Value4();
		constexpr Value4(T x, T y, T z, T w);
		constexpr explicit Value4(T scalar);

		constexpr bool operator == (const Value4& other) const = default;
		constexpr bool operator != (const Value4& other) const = default;

		T X, Y, Z, W;
	};

	typedef Value4<float> Float4;
	typedef Value4<int32_t> Int4;
	typedef Value4<uint32_t> Uint4;
}

namespace Math
{
	template <typename T>
	constexpr Value4<T>::Value4():
		X(T{}), Y(T{}), Z(T{}), W(T{})
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const T x, const T y, const T z, const T w):
			X(x), Y(y), Z(z), W(w)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const T scalar):
		X(scalar), Y(scalar), Z(scalar), W(scalar)
	{
	}
}