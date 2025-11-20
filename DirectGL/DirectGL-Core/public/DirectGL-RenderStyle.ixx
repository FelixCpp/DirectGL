// Project Name : DirectGL
// File Name    : DirectGL-RenderStyle.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

export module DirectGL:RenderStyle;

import :Color;
import :StrokeJoin;
import :StrokeCap;
import :StrokeAlignment;
import :RectMode;
import :EllipseMode;
import :EllipseSegmentsMode;
import :MatrixStack;
import :BlendMode;
import :Shader;
import :ClipRect;

import :Font;
import :TextAlignment;

import :ImageSampler;

export namespace DGL
{
	struct RenderStyle
	{
		color_t	FillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool	IsFillEnabled = true;

		color_t StrokeColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		bool	IsStrokeEnabled = true;
		float	StrokeWeight = 1.0f;

		color_t				ImageTint		= { 1.0f, 1.0f, 1.0f, 1.0f };
		const ImageSampler* ImageSampler	= nullptr;
		RectMode			ImageMode		= RectModeLTWH();

		Font* Font				= nullptr;
		float TextSize			= 12.0f;
		TextAlignment TextAlign	= TextAlignment::TopLeft;

		Shader*	Shader = nullptr;

		StrokeJoin		JoinStyle		= StrokeJoin::Miter;
		StrokeCap		StrokeCap		= StrokeCap::Round;
		StrokeAlignment StrokeAlignment	= StrokeAlignment::Center;

		ClipRect	ClipRect = ClipRect::Unclipped();

		RectMode			ClipRectMode		= RectModeLTWH();
		RectMode			RectMode			= RectModeLTWH();
		EllipseMode			EllipseMode			= EllipseModeCenterRadius();
		EllipseSegmentsMode EllipseSegmentsMode	= EllipseSegmentsModeAdaptive();

		BlendMode BlendMode = BlendMode::Alpha;
		MatrixStack MatrixStack;
	};
}