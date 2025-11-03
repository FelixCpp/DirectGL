// Project Name : DirectGL
// File Name    : DirectGL-RenderStyle.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

export module DirectGL:RenderStyle;

import :Color;

export namespace DGL
{
	struct RenderStyle
	{
		color_t	FillColor;
		bool	IsFillEnabled;

		color_t StrokeColor;
		bool	IsStrokeEnabled;
		float	StrokeWeight;
	};
}