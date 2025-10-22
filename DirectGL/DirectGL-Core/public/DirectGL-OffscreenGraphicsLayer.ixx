// Project Name : DirectGL-Core
// File Name    : DirectGL-OffscreenGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:OffscreenGraphicsLayer;

import DirectGL.Blending;
import DirectGL.Renderer;
import DirectGL.Math;
import DirectGL.Texture;

import :BaseGraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class OffscreenGraphicsLayer : public GraphicsLayer
	{
	public:

		static std::unique_ptr<OffscreenGraphicsLayer> Create(
			Math::Uint2 viewportSize,
			RendererFacade& renderer,
			Blending::BlendModeActivator& blendModeActivator
		);

		void BeginDraw();
		void EndDraw();

		void Resume();
		void Suspend();

		const Texture::Texture& GetRenderTexture() const;

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

		explicit OffscreenGraphicsLayer(
			Math::Uint2 viewportSize,
			RendererFacade& renderer,
			Blending::BlendModeActivator& blendModeActivator
		);

		std::unique_ptr<Renderer::OffscreenRenderTarget> m_RenderTarget;
		BaseGraphicsLayer m_GraphicsLayerImpl;

	};
}