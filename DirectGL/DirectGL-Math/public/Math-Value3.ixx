// Project Name : DirectGL-Math
// File Name    : Math-Value3.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

module;

#include <cstdint>

export module DirectGL.Math:Value3;

export namespace DGL::Math
{
	template <typename T>
	struct Value3
	{
		T X, Y, Z;
	};

	typedef Value3<float> Float3;
	typedef Value3<int32_t> Int3;
	typedef Value3<uint32_t> Uint3;
}