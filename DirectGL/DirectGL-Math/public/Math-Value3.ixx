// Project Name : DirectGL-Math
// File Name    : Math-Value3.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

module;

#include <cstdint>
#include <concepts>

export module DirectGL.Math:Value3;

import :Value2;

export namespace DGL::Math
{
	template <typename T>
	struct Value3
	{
		constexpr Value3();
		constexpr Value3(T x, T y, T z);
		constexpr explicit Value3(T scalar);

		template <std::convertible_to<T> U>
		constexpr explicit Value3(const Value3<U>& other) :
			X(static_cast<T>(other.X)),
			Y(static_cast<T>(other.Y)),
			Z(static_cast<T>(other.Z))
		{
		}

		constexpr explicit Value3(Value2<T> xy, T z);
		constexpr explicit Value3(T x, Value2<T> yz);

		T X, Y, Z;
	};

	typedef Value3<float> Float3;
	typedef Value3<int32_t> Int3;
	typedef Value3<uint32_t> Uint3;
}

namespace DGL::Math
{
	template <typename T>
	constexpr Value3<T>::Value3():
		X(T{}), Y(T{}), Z(T{})
	{
	}

	template <typename T>
	constexpr Value3<T>::Value3(T x, T y, T z) :
		X(x), Y(y), Z(z)
	{
	}

	template <typename T>
	constexpr Value3<T>::Value3(T scalar):
		X(scalar), Y(scalar), Z(scalar)
	{
	}

	template <typename T>
	constexpr Value3<T>::Value3(Value2<T> xy, T z):
		X(xy.X), Y(xy.Y), Z(z)
	{
	}

	template <typename T>
	constexpr Value3<T>::Value3(T x, Value2<T> yz):
		X(x), Y(yz.X), Z(yz.Y)
	{
	}
}
