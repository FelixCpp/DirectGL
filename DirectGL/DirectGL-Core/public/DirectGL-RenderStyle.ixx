// Project Name : DirectGL
// File Name    : DirectGL-RenderStyle.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

export module DirectGL:RenderStyle;

import :Color;
import :StrokeJoin;
import :StrokeCap;
import :RectMode;
import :EllipseMode;
import :EllipseSegmentsMode;
import :MatrixStack;
import :BlendMode;

export namespace DGL
{
	struct RenderStyle
	{
		color_t	FillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool	IsFillEnabled = true;

		color_t StrokeColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool	IsStrokeEnabled = true;
		float	StrokeWeight = 1.0f;

		StrokeJoin	JoinStyle = StrokeJoin::Miter;
		StrokeCap	StrokeCap = StrokeCap::Round;

		RectMode RectMode = RectModeLTWH();
		EllipseMode EllipseMode = EllipseModeCenterRadius();
		EllipseSegmentsMode EllipseSegmentsMode = EllipseSegmentsModeAdaptive();

		BlendMode BlendMode = BlendMode::Alpha;
		MatrixStack MatrixStack;
	};
}