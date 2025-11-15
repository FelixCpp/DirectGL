// Project Name : DirectGL
// File Name    : DirectGL-TextAlignment.ixx
// Author       : Felix Busch
// Created Date : 2025/11/13

export module DirectGL:TextAlignment;

export namespace DGL
{
	enum class HorizontalTextAlignment
	{
		Left, 	 // Align text to the left
		Center,  // Center text horizontally
		Right    // Align text to the right
	};

	enum class VerticalTextAlignment
	{
		Top,     // Align text to the top
		Center,  // Center text vertically
		Bottom   // Align text to the bottom
	};

	struct TextAlignment
	{
		HorizontalTextAlignment Horizontal;
		VerticalTextAlignment   Vertical;

		static const TextAlignment TopLeft;
		static const TextAlignment TopCenter;
		static const TextAlignment TopRight;

		static const TextAlignment CenterLeft;
		static const TextAlignment Center;
		static const TextAlignment CenterRight;

		static const TextAlignment BottomLeft;
		static const TextAlignment BottomCenter;
		static const TextAlignment BottomRight;
	};
}

namespace DGL
{
	inline constexpr TextAlignment TextAlignment::TopLeft = { HorizontalTextAlignment::Left,   VerticalTextAlignment::Top };
	inline constexpr TextAlignment TextAlignment::TopCenter = { HorizontalTextAlignment::Center, VerticalTextAlignment::Top };
	inline constexpr TextAlignment TextAlignment::TopRight = { HorizontalTextAlignment::Right, VerticalTextAlignment::Top };
	inline constexpr TextAlignment TextAlignment::CenterLeft = { HorizontalTextAlignment::Left,   VerticalTextAlignment::Center };
	inline constexpr TextAlignment TextAlignment::Center = { HorizontalTextAlignment::Center, VerticalTextAlignment::Center };
	inline constexpr TextAlignment TextAlignment::CenterRight = { HorizontalTextAlignment::Right, VerticalTextAlignment::Center };
	inline constexpr TextAlignment TextAlignment::BottomLeft = { HorizontalTextAlignment::Left,   VerticalTextAlignment::Bottom };
	inline constexpr TextAlignment TextAlignment::BottomCenter = { HorizontalTextAlignment::Center, VerticalTextAlignment::Bottom };
	inline constexpr TextAlignment TextAlignment::BottomRight = { HorizontalTextAlignment::Right, VerticalTextAlignment::Bottom };
}