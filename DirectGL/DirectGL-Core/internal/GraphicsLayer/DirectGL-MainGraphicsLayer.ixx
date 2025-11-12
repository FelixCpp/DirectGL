// Project Name : DirectGL
// File Name    : DirectGL-MainGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <memory>
#include <string_view>

export module DirectGL:MainGraphicsLayer;

import :GraphicsLayer;
import :RenderStyleStack;
import :Renderer2D;
import :MainRenderTarget;

namespace DGL
{
	class MainGraphicsLayer : public GraphicsLayer
	{
	public:

		explicit MainGraphicsLayer(
			const std::weak_ptr<Renderer2D>& renderer,
			const Math::FloatBoundary& viewport
		);

		void SetViewport(const Math::FloatBoundary& viewport);
		const Math::FloatBoundary& GetViewport() const;

		void BeginDraw();
		void EndDraw();

		void PushStyle(bool extendCurrentStyle) override;
		void PopStyle() override;
		RenderStyle& PeekStyle() override;

		void PushMatrix(bool extendCurrentMatrix) override;
		void PopMatrix() override;
		Math::Matrix4x4& PeekMatrix() override;

		void Translate(float x, float y) override;
		void Rotate(Math::Angle angle) override;
		void Scale(float scaleX, float scaleY) override;
		void Shear(Math::Angle shearX, Math::Angle shearY) override;

		void SetRectMode(const RectMode& mode) override;
		void SetEllipseMode(const EllipseMode& mode) override;
		void SetEllipseSegmentsMode(const EllipseSegmentsMode& mode) override;

		void SetFillColor(color_t color) override;
		void SetFillColorDisabled() override;

		void SetStrokeColor(color_t color) override;
		void SetStrokeColorDisabled() override;
		void SetStrokeWeight(float weight) override;
		void SetStrokeJoin(StrokeJoin joinStyle) override;
		void SetStrokeCap(StrokeCap strokeCap) override;

		void SetBlendMode(const BlendMode& blendMode) override;
		void SetClipRectMode(const RectMode& mode) override;
		void SetClipRect(float x1, float y1, float x2, float y2) override;
		void SetClipRectDisabled() override;

		void SetTextSize(float textSize) override;
		void SetTextFont(const Font* font) override;

		void BeginShape(ShapeMode mode) override;
		void EndShape(ShapeClosingMode mode) override;
		void Vertex(float x, float y) override;

		void Background(color_t color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
		void Text(std::string_view text, float x, float y) override;

	private:

		RenderStyleStack m_RenderStyleStack;
		std::weak_ptr<Renderer2D> m_Renderer;
		MainRenderTarget m_RenderTarget;

	};
}