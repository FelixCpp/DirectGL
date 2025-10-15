// Project Name : DirectGL-Core
// File Name    : DirectGL-OffscreenGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:OffscreenGraphicsLayer;

import DirectGL.Renderer;
import DirectGL.Math;

import :BaseGraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class OffscreenGraphicsLayer : public Renderer::RenderTarget, public GraphicsLayer
	{
	public:

		static std::unique_ptr<OffscreenGraphicsLayer> Create(
			Uint2 viewportSize,
			RendererFacade& renderer,
			ShapeRenderer::ShapeFactory& shapeFactory
		);

		void BeginDraw() override;
		void EndDraw() override;

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

		explicit OffscreenGraphicsLayer(
			Uint2 viewportSize,
			RendererFacade& renderer,
			ShapeRenderer::ShapeFactory& shapeFactory
		);

		std::unique_ptr<Renderer::OffscreenRenderTarget> m_RenderTarget;
		BaseGraphicsLayer m_GraphicsLayerImpl;

	};
}