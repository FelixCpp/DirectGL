// Project Name : DirectGL
// File Name    : DirectGL-RenderStyle.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

export module DirectGL:RenderStyle;

import :Color;
import :StrokeJoin;
import :StrokeCap;

export namespace DGL
{
	struct RenderStyle
	{
		color_t	FillColor;
		bool	IsFillEnabled = true;

		color_t StrokeColor;
		bool	IsStrokeEnabled = true;
		float	StrokeWeight = 1.0f;

		StrokeJoin	JoinStyle = StrokeJoin::Miter;
		StrokeCap	StartCap = StrokeCap::Round;
		StrokeCap	EndCap = StrokeCap::Round;
	};
}