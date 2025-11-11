// Project Name : Math
// File Name    : Math-Value4.ixx
// Author       : Felix Busch
// Created Date : 2025/09/23

module;

#include <cstdint>

export module DirectGL.Math:Value4;

import :Value2;
import :Value3;

export namespace DGL::Math
{
	template <typename T>
	struct Value4
	{
		constexpr Value4();
		constexpr Value4(T x, T y, T z, T w);
		constexpr explicit Value4(T scalar);

		constexpr Value4(Value2<T> xy, T z, T w);
		constexpr Value4(T x, Value2<T> yz, T w);
		constexpr Value4(T x, T y, Value2<T> zw);

		constexpr Value4(Value2<T> xy, Value2<T> zw);

		constexpr Value4(const Value3<T>& xyz, T w);
		constexpr Value4(T x, const Value3<T>& yzw);

		constexpr bool operator == (const Value4& other) const = default;
		constexpr bool operator != (const Value4& other) const = default;

		T X, Y, Z, W;
	};

	typedef Value4<float> Float4;
	typedef Value4<int32_t> Int4;
	typedef Value4<uint32_t> Uint4;
}

namespace DGL::Math
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

	template <typename T>
	constexpr Value4<T>::Value4(const Value2<T> xy, const T z, const T w):
		X(xy.X), Y(xy.Y), Z(z), W(w)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const T x, const Value2<T> yz, const T w):
		X(x), Y(yz.X), Z(yz.Y), W(w)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const T x, const T y, const Value2<T> zw):
		X(x), Y(y), Z(zw.X), W(zw.Y)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const Value2<T> xy, const Value2<T> zw):
		X(xy.X), Y(xy.Y), Z(zw.X), W(zw.Y)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const Value3<T>& xyz, const T w):
		X(xyz.X), Y(xyz.Y), Z(xyz.Z), W(w)
	{
	}

	template <typename T>
	constexpr Value4<T>::Value4(const T x, const Value3<T>& yzw):
		X(x), Y(yzw.X), Z(yzw.Y), W(yzw.Z)
	{
	}
}