// Project Name : DirectGL
// File Name    : DirectGL-GraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <string_view>

export module DirectGL:GraphicsLayer;

import :RenderStyle;
import :ShapeMode;
import :Font;

namespace DGL
{
	/**
	 * A GraphicsLayer is an interface encapsulating RenderStyle-access as well as
	 * shape drawing functionality.
	 */
	struct GraphicsLayer
	{
		virtual ~GraphicsLayer() = default;

		virtual void PushStyle(bool extendCurrentStyle = true) = 0;
		virtual void PopStyle() = 0;
		virtual RenderStyle& PeekStyle() = 0;

		virtual void PushMatrix(bool extendCurrentMatrix = true) = 0;
		virtual void PopMatrix() = 0;
		virtual Math::Matrix4x4& PeekMatrix() = 0;

		virtual void Translate(float x, float y) = 0;
		virtual void Rotate(Math::Angle angle) = 0;
		virtual void Scale(float scaleX, float scaleY) = 0;
		virtual void Shear(Math::Angle shearX, Math::Angle shearY) = 0;

		virtual void SetRectMode(const RectMode& mode) = 0;
		virtual void SetEllipseMode(const EllipseMode& mode) = 0;
		virtual void SetEllipseSegmentsMode(const EllipseSegmentsMode& mode) = 0;

		virtual void SetFillColor(color_t color) = 0;
		virtual void SetFillColorDisabled() = 0;

		virtual void SetStrokeColor(color_t color) = 0;
		virtual void SetStrokeColorDisabled() = 0;
		virtual void SetStrokeWeight(float weight) = 0;
		virtual void SetStrokeJoin(StrokeJoin joinStyle) = 0;
		virtual void SetStrokeCap(StrokeCap strokeCap) = 0;

		virtual void SetBlendMode(const BlendMode& blendMode) = 0;
		virtual void SetClipRectMode(const RectMode& mode) = 0;
		virtual void SetClipRect(float x1, float y1, float x2, float y2) = 0;
		virtual void SetClipRectDisabled() = 0;

		virtual void SetTextSize(float textSize) = 0;
		virtual void SetTextFont(const Font* font) = 0;

		virtual void BeginShape(ShapeMode mode) = 0;
		virtual void EndShape(ShapeClosingMode mode) = 0;
		virtual void Vertex(float x, float y) = 0;

		virtual void Background(color_t color) = 0;
		virtual void Rect(float x1, float y1, float x2, float y2) = 0;
		virtual void Ellipse(float x1, float y1, float x2, float y2) = 0;
		virtual void Point(float x, float y) = 0;
		virtual void Line(float x1, float y1, float x2, float y2) = 0;
		virtual void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
		virtual void Text(std::string_view text, float x, float y) = 0;
	};
}