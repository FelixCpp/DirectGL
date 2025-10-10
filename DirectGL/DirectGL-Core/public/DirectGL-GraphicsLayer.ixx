// Project Name : DirectGL-Core
// File Name    : DirectGL-GraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

export module DirectGL:GraphicsLayer;

import :Color;
import :BlendMode;
import :RenderState;

export namespace DGL
{
	struct GraphicsLayer
	{
		virtual ~GraphicsLayer() = default;

		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;

		virtual void PushState() = 0;
		virtual void PopState() = 0;
		virtual RenderState& PeekState() = 0;

		virtual void Fill(Color color) = 0;
		virtual void Stroke(Color color) = 0;
		virtual void StrokeWeight(float strokeWeight) = 0;

		virtual void NoFill() = 0;
		virtual void NoStroke() = 0;

		virtual void Blend(const BlendMode& blendMode) = 0;

		virtual void Background(Color color) = 0;
		virtual void Rect(float x1, float y1, float x2, float y2) = 0;
		virtual void Ellipse(float x1, float y1, float x2, float y2) = 0;
		virtual void Circle(float x, float y, float diameter) = 0;
		virtual void Line(float x1, float y1, float x2, float y2) = 0;
		virtual void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
	};
}