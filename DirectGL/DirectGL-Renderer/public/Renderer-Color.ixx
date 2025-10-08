// Project Name : DirectGL-Renderer
// File Name    : Renderer-Color.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <cstdint>

export module DirectGL.Renderer:Color;

import Math;

export namespace DGL::Renderer
{
	struct Color
	{
		constexpr Color();
		constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
		constexpr explicit Color(uint32_t rgba);

		constexpr bool operator == (Color other) const;
		constexpr bool operator != (Color other) const;

		constexpr Color Lerp(Color other, float t) const;

		uint8_t R, G, B, A;
	};
}

namespace DGL::Renderer
{
	constexpr Color::Color():
		R(255), G(255), B(255), A(255)
	{
	}

	constexpr Color::Color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha):
		R(red), G(green), B(blue), A(alpha)
	{
	}

	constexpr Color::Color(const uint32_t rgba):
		R((rgba & 0xFF000000) >> 24),
		G((rgba & 0x00FF0000) >> 16),
		B((rgba & 0x0000FF00) >> 8),
		A((rgba & 0x000000FF) >> 0)
	{
	}

	constexpr bool Color::operator==(const Color other) const
	{
		return (R == other.R) && (G == other.G) && (B == other.B) && (A == other.A);
	}

	constexpr bool Color::operator!=(const Color other) const
	{
		return !(*this == other);
	}

	constexpr Color Color::Lerp(const Color other, const float t) const
	{
		return Color(
			static_cast<uint8_t>(Math::Lerp(R, other.R, t)),
			static_cast<uint8_t>(Math::Lerp(G, other.G, t)),
			static_cast<uint8_t>(Math::Lerp(B, other.B, t)),
			static_cast<uint8_t>(Math::Lerp(A, other.A, t))
		);
	}


}
