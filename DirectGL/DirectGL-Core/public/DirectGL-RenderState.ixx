// Project Name : DirectGL-Core
// File Name    : DirectGL-RenderState.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

export module DirectGL:RenderState;

import DirectGL.Renderer;
import DirectGL.Blending;
import DirectGL.ShapeRenderer;
import DirectGL.Texture;

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
		Texture::TextureFilterMode ImageFilterMode;
		Texture::TextureWrapMode ImageWrapMode;
		uint8_t ImageAlpha;

		Blending::BlendMode BlendMode;
		RectMode ImageMode;
		RectMode RectMode;
		EllipseMode EllipseMode;
		SegmentCountMode EllipseSegmentCountMode;

		ShapeRenderer::LineCapStyle LineStartCap;
		ShapeRenderer::LineCapStyle LineEndCap;
		ShapeRenderer::LineJoinStyle LineJoinStyle;
		SegmentCountMode LineSegmentCountMode;

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
		IsFillEnabled(true),
		IsStrokeEnabled(true),
		ImageTint(Renderer::Colors::White),
		ImageFilterMode(Texture::TextureFilterMode::Linear),
		ImageWrapMode(Texture::TextureWrapMode::ClampToEdge),
		ImageAlpha(255),
		BlendMode(Blending::BlendModes::Alpha),
		ImageMode(RectModeLTWH()),
		RectMode(RectModeLTWH()),
		EllipseMode(EllipseModeCenterDiameter()),
		EllipseSegmentCountMode(SegmentCountModeSmooth()),
		LineStartCap(ShapeRenderer::LineCapStyle::Butt),
		LineEndCap(ShapeRenderer::LineCapStyle::Butt),
		LineJoinStyle(ShapeRenderer::LineJoinStyle::Miter),
		LineSegmentCountMode(SegmentCountModeSmooth())
	{
	}
}
