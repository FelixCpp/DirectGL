// Project Name : DirectGL-Core
// File Name    : DirectGL-GraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

export module DirectGL:GraphicsLayer;

import DirectGL.Math;
import DirectGL.Blending;
import DirectGL.Renderer;
import DirectGL.Texture;

import :BlendMode;
import :RenderState;
import :DrawMode;

export namespace DGL
{
	struct GraphicsLayer
	{
		virtual ~GraphicsLayer() = default;

		virtual const Math::FloatBoundary& GetViewport() const = 0;

		virtual void PushState() = 0;
		virtual void PopState() = 0;
		virtual RenderState& PeekState() = 0;

		virtual void PushTransform() = 0;
		virtual void PopTransform() = 0;
		virtual Math::Matrix4x4& PeekTransform() = 0;
		virtual void ResetTransform() = 0;

		virtual void Translate(float x, float y) = 0;
		virtual void Scale(float x, float y) = 0;
		virtual void Rotate(Math::Angle angle) = 0;
		virtual void Skew(Math::Angle angleX, Math::Angle angleY) = 0;
		
		virtual void Fill(Renderer::Color color) = 0;
		virtual void Stroke(Renderer::Color color) = 0;
		virtual void StrokeWeight(float strokeWeight) = 0;

		virtual void NoFill() = 0;
		virtual void NoStroke() = 0;

		virtual void SetBlendMode(const Blending::BlendMode& blendMode) = 0;
		virtual void SetRectMode(const RectMode& rectMode) = 0;
		virtual void SetImageMode(const RectMode& imageMode) = 0;
		virtual void SetEllipseMode(const EllipseMode& ellipseMode) = 0;
		virtual void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) = 0;

		virtual void SetImageTint(Renderer::Color tint) = 0;
		virtual void SetImageAlpha(uint8_t alpha) = 0;
		virtual void SetImageOpacity(float opacity) = 0;

		virtual void Background(Renderer::Color color) = 0;
		virtual void Rect(float x1, float y1, float x2, float y2) = 0;
		virtual void Ellipse(float x1, float y1, float x2, float y2) = 0;
		virtual void Point(float x, float y) = 0;
		virtual void Line(float x1, float y1, float x2, float y2) = 0;
		virtual void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void Image(const Texture::Texture& texture, float x1, float y1, float x2, float y2) = 0;
	};
}