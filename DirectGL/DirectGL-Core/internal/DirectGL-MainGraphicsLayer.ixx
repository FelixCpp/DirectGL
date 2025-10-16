// Project Name : DirectGL-Core
// File Name    : DirectGL-MainGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:MainGraphicsLayer;

import DirectGL.Blending;
import DirectGL.Renderer;
import DirectGL.ShapeRenderer;
import DirectGL.Math;

import :BaseGraphicsLayer;
import :RenderStateStack;
import :RendererFacade;

export namespace DGL
{
	class MainGraphicsLayer : public Renderer::RenderTarget, public GraphicsLayer
	{
	public:

		static std::unique_ptr<MainGraphicsLayer> Create(
			Math::Uint2 viewportSize,
			RendererFacade& renderer,
			ShapeRenderer::ShapeFactory& shapeFactory,
			Blending::BlendModeActivator& blendModeActivator
		);

		void Resize(Math::Uint2 viewportSize);

		void BeginDraw() override;
		void EndDraw() override;

		void PushState() override;
		void PopState() override;
		RenderState& PeekState() override;

		void PushTransform() override;
		void PopTransform() override;
		Math::Matrix4x4& PeekTransform() override;

		void ResetTransform() override;
		void Translate(float x, float y) override;
		void Scale(float x, float y) override;
		void Rotate(Math::Angle angle) override;
		void Skew(Math::Angle angleX, Math::Angle angleY) override;

		void Fill(Renderer::Color color) override;
		void Stroke(Renderer::Color color) override;
		void StrokeWeight(float strokeWeight) override;

		void NoFill() override;
		void NoStroke() override;

		void SetBlendMode(const Blending::BlendMode& blendMode) override;
		void SetRectMode(const DGL::RectMode& rectMode) override;
		void SetImageMode(const DGL::RectMode& imageMode) override;
		void SetEllipseMode(const DGL::EllipseMode& ellipseMode) override;
		void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) override;

		void Background(Renderer::Color color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
		void Image(const Texture::Texture& texture, float x1, float y1, float x2, float y2) override;

	private:

		explicit MainGraphicsLayer(
			Math::Uint2 viewportSize,
			RendererFacade& renderer,
			ShapeRenderer::ShapeFactory& shapeFactory,
			Blending::BlendModeActivator& blendModeActivator
		);

		std::unique_ptr<Renderer::MainRenderTarget> m_MainRenderTarget;
		BaseGraphicsLayer m_GraphicsLayer;

	};
}