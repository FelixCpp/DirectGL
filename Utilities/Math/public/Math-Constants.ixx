// Project Name : Math
// File Name    : Math-Constants.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <numbers>

export module Math:Constants;

export namespace Math
{
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr float TAU = 2.0f * PI;
}