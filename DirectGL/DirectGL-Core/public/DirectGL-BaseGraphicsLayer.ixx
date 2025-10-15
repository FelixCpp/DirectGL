// Project Name : DirectGL-Core
// File Name    : DirectGL-BaseGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:BaseGraphicsLayer;

import :RendererFacade;
import :RenderStateStack;
import :GraphicsLayer;
import :Math;

export namespace DGL
{
	class BaseGraphicsLayer : public GraphicsLayer
	{
	public:

		explicit BaseGraphicsLayer(
			RendererFacade& renderer,
			ShapeRenderer::ShapeFactory& shapeFactory
		);

		void SetViewport(FloatBoundary viewport);
		const FloatBoundary& GetViewport() const;

		void BeginDraw();
		void EndDraw();

		void PushState() override;
		void PopState() override;
		RenderState& PeekState() override;

		void PushTransform() override;
		void PopTransform() override;
		Matrix4x4& PeekTransform() override;
		void ResetTransform() override;

		void Translate(float x, float y) override;
		void Scale(float x, float y) override;
		void Rotate(Angle angle) override;
		void Skew(Angle angleX, Angle angleY) override;

		void Fill(Color color) override;
		void Stroke(Color color) override;
		void StrokeWeight(float strokeWeight) override;

		void NoFill() override;
		void NoStroke() override;

		void SetBlendMode(const BlendMode& blendMode) override;
		void SetRectMode(const DGL::RectMode& rectMode) override;
		void SetImageMode(const DGL::RectMode& imageMode) override;
		void SetEllipseMode(const DGL::EllipseMode& ellipseMode) override;
		void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) override;

		void Background(Color color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
		void Image(const Texture& texture, float x1, float y1, float x2, float y2) override;

	private:

		RendererFacade* m_Renderer;
		ShapeRenderer::ShapeFactory* m_ShapeFactory;

		std::unique_ptr<Renderer::SolidColorBrush> m_SolidFillBrush;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidStrokeBrush;

		std::unique_ptr<Renderer::TextureBrush> m_TextureFillBrush;

		RenderStateStack m_RenderStates;

		FloatBoundary m_Viewport;
		Matrix4x4 m_ProjectionMatrix;

	};
}