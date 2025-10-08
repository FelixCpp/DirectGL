// Project Name : DirectGL-Input
// File Name    : Input-MouseButton.ixx
// Author       : Felix Busch
// Created Date : 2025/10/09

export module DirectGL.Input:MouseButton;

export namespace DGL::Input
{
	enum class MouseButton
	{
		Left,
		Right,
		Middle,
		Extra1,
		Extra2,
	};

	inline constexpr size_t MouseButtonCount = static_cast<int>(MouseButton::Extra2) + 1;
}