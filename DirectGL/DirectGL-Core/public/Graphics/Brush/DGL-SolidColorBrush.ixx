// Project Name : DirectGL
// File Name    : DGL-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:SolidColorBrush;

import :Brush;
import :Color;

export namespace DGL
{
	struct SolidColorBrush : Brush
	{
		~SolidColorBrush() override = default;

		virtual void SetColor(Color color) = 0;
		[[nodiscard]] virtual Color GetColor() const = 0;
	};
}