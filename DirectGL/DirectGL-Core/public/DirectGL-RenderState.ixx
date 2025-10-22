// Project Name : DirectGL-Core
// File Name    : DirectGL-RenderState.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

export module DirectGL:RenderState;

import DirectGL.Renderer;
import DirectGL.Blending;
import DirectGL.ShapeRenderer;

import :TransformationStack;
import :DrawMode;

export namespace DGL
{
	struct RenderState
	{
		Renderer::Color FillColor;
		Renderer::Color StrokeColor;
		float StrokeWeight;

		bool IsFillEnabled;
		bool IsStrokeEnabled;

		Renderer::Color ImageTint;
		uint8_t ImageAlpha;

		Blending::BlendMode BlendMode;
		RectMode ImageMode;
		RectMode RectMode;
		EllipseMode EllipseMode;
		SegmentCountMode SegmentCountMode;

		ShapeRenderer::LineCapStyle StartCap;
		ShapeRenderer::LineCapStyle EndCap;

		TransformationStack TransformationStack;

		RenderState();
	};
}

namespace DGL
{
	RenderState::RenderState():
		FillColor(255, 255, 255),
		StrokeColor(255, 255, 255),
		StrokeWeight(1.0f),
		ImageAlpha(255),
		IsFillEnabled(true),
		IsStrokeEnabled(true),
		BlendMode(Blending::BlendModes::Alpha),
		ImageMode(RectModeLTWH()),
		RectMode(RectModeLTWH()),
		EllipseMode(EllipseModeCenterDiameter()),
		SegmentCountMode(SegmentCountModeSmooth()),
		StartCap(ShapeRenderer::LineCapStyle::Butt),
		EndCap(ShapeRenderer::LineCapStyle::Butt)
	{
	}
}
