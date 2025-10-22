// Project Name : DirectGL-Core
// File Name    : DirectGL-BaseGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:BaseGraphicsLayer;

import DirectGL.Math;
import DirectGL.Renderer;
import DirectGL.Brushes;
import DirectGL.Blending;

import :RendererFacade;
import :RenderStateStack;
import :GraphicsLayer;
import :DepthProvider;

export namespace DGL
{
	class BaseGraphicsLayer : public GraphicsLayer
	{
	public:

		explicit BaseGraphicsLayer(
			RendererFacade& renderer,
			Math::Uint2 viewportSize,
			Blending::BlendModeActivator& blendModeActivator,
			std::unique_ptr<DepthProvider> depthProvider
		);

		void SetViewport(Math::FloatBoundary viewport);
		const Math::FloatBoundary& GetViewport() const;

		void BeginDraw();
		void EndDraw();

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
		void SetRectMode(const RectMode& rectMode) override;
		void SetImageMode(const RectMode& imageMode) override;
		void SetEllipseMode(const EllipseMode& ellipseMode) override;
		void SetSegmentCountMode(const SegmentCountMode& segmentCountMode) override;

		void Background(Renderer::Color color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;
		void Image(const Texture::Texture& texture, float x1, float y1, float x2, float y2) override;

	private:

		float IncrementAndGetDepth() const;

		RendererFacade* m_Renderer;
		Blending::BlendModeActivator* m_BlendModeActivator;

		std::unique_ptr<Brushes::SolidColorBrush> m_SolidFillBrush;
		std::unique_ptr<Brushes::SolidColorBrush> m_SolidStrokeBrush;

		std::unique_ptr<Brushes::TextureBrush> m_TextureFillBrush;
		std::unique_ptr<DepthProvider> m_DepthProvider;

		RenderStateStack m_RenderStates;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

	};
}