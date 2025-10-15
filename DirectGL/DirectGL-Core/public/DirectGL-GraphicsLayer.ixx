// Project Name : DirectGL-Core
// File Name    : DirectGL-GraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

export module DirectGL:GraphicsLayer;

import :Color;
import :BlendMode;
import :RenderState;
import :Texture;
import :DrawMode;
import :Math;

export namespace DGL
{
	struct GraphicsLayer
	{
		virtual ~GraphicsLayer() = default;

		virtual void PushState() = 0;
		virtual void PopState() = 0;
		virtual RenderState& PeekState() = 0;

		virtual void PushTransform() = 0;
		virtual void PopTransform() = 0;
		virtual Matrix4x4& PeekTransform() = 0;
		virtual void ResetTransform() = 0;

		virtual void Translate(float x, float y) = 0;
		virtual void Scale(float x, float y) = 0;
		virtual void Rotate(Angle angle) = 0;
		virtual void Skew(Angle angleX, Angle angleY) = 0;
		
		virtual void Fill(Color color) = 0;
		virtual void Stroke(Color color) = 0;
		virtual void StrokeWeight(float strokeWeight) = 0;

		virtual void NoFill() = 0;
		virtual void NoStroke() = 0;

		virtual void SetBlendMode(const BlendMode& blendMode) = 0;
		virtual void SetRectMode(const RectMode& rectMode) = 0;
		virtual void SetEllipseMode(const EllipseMode& ellipseMode) = 0;
		virtual void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) = 0;

		virtual void Background(Color color) = 0;
		virtual void Rect(float x1, float y1, float x2, float y2) = 0;
		virtual void Ellipse(float x1, float y1, float x2, float y2) = 0;
		virtual void Point(float x, float y) = 0;
		virtual void Line(float x1, float y1, float x2, float y2) = 0;
		virtual void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void Image(const Texture& texture, float x1, float y1, float x2, float y2) = 0;
	};
}