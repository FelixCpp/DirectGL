// Project Name : DirectGL
// File Name    : DGL-Color.ixx
// Author       : Felix Busch
// Created Date : 2025/09/23

module;

#include <cstdint>

export module DGL:Color;

export namespace DGL
{
	struct Color
	{
		constexpr Color();
		constexpr Color(int32_t r, int32_t g, int32_t b, int32_t a = 255);
		constexpr explicit Color(int32_t gray, int32_t a = 255);

		constexpr Color WithAlpha(int32_t alpha) const;

		constexpr bool operator == (const Color& other) const = default;
		constexpr bool operator != (const Color& other) const = default;

		uint8_t R, G, B, A;
	};
}

namespace DGL
{
	constexpr Color::Color():
		R(255), G(255), B(255), A(255)
	{
	}

	constexpr Color::Color(const int32_t r, const int32_t g, const int32_t b, const int32_t a):
		R(r), G(g), B(b), A(a)
	{
	}

	constexpr Color::Color(const int32_t gray, const int32_t a):
		R(gray), G(gray), B(gray), A(a)
	{
	}

	constexpr Color Color::WithAlpha(int32_t alpha) const
	{
		return { R, G, B, alpha };
	}
}