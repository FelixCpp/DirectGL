module;

#include <glad/gl.h>

#include <memory>
#include <type_traits>

module DirectGL;

import :OffscreenGraphicsLayer;

namespace DGL
{
	std::unique_ptr<OffscreenGraphicsLayer> OffscreenGraphicsLayer::Create(
		const Math::Uint2 viewportSize,
		RendererFacade& renderer,
		Blending::BlendModeActivator& blendModeActivator
	) {
		return std::unique_ptr<OffscreenGraphicsLayer>(new OffscreenGraphicsLayer(viewportSize, renderer, blendModeActivator));
	}

	void OffscreenGraphicsLayer::BeginDraw()
	{
		m_GraphicsLayerImpl.BeginDraw();
		m_RenderTarget->Activate();
	}

	void OffscreenGraphicsLayer::EndDraw()
	{
		m_GraphicsLayerImpl.EndDraw();
	}

	void OffscreenGraphicsLayer::Resume()
	{
		m_RenderTarget->Activate();
	}

	void OffscreenGraphicsLayer::Suspend()
	{
		// Nothing to do for now
	}

	const Texture::Texture& OffscreenGraphicsLayer::GetRenderTexture() const
	{
		return m_RenderTarget->GetRenderTexture();
	}

	const Math::FloatBoundary& OffscreenGraphicsLayer::GetViewport() const
	{
		return m_GraphicsLayerImpl.GetViewport();
	}

	void OffscreenGraphicsLayer::PushState() { m_GraphicsLayerImpl.PushState(); }
	void OffscreenGraphicsLayer::PopState() { m_GraphicsLayerImpl.PopState(); }
	RenderState& OffscreenGraphicsLayer::PeekState() { return m_GraphicsLayerImpl.PeekState(); }

	void OffscreenGraphicsLayer::PushTransform() { m_GraphicsLayerImpl.PushTransform(); }
	void OffscreenGraphicsLayer::PopTransform() { m_GraphicsLayerImpl.PopTransform(); }
	Math::Matrix4x4& OffscreenGraphicsLayer::PeekTransform() { return m_GraphicsLayerImpl.PeekTransform(); }

	void OffscreenGraphicsLayer::ResetTransform() { m_GraphicsLayerImpl.ResetTransform(); }
	void OffscreenGraphicsLayer::Translate(const float x, const float y) { m_GraphicsLayerImpl.Translate(x, y); }
	void OffscreenGraphicsLayer::Scale(const float x, const float y) { m_GraphicsLayerImpl.Scale(x, y); }
	void OffscreenGraphicsLayer::Rotate(const Math::Angle angle) { m_GraphicsLayerImpl.Rotate(angle); }
	void OffscreenGraphicsLayer::Skew(const Math::Angle angleX, const Math::Angle angleY) { m_GraphicsLayerImpl.Skew(angleX, angleY); }

	void OffscreenGraphicsLayer::Fill(const Renderer::Color color) { m_GraphicsLayerImpl.Fill(color); }
	void OffscreenGraphicsLayer::Stroke(const Renderer::Color color) { m_GraphicsLayerImpl.Stroke(color); }
	void OffscreenGraphicsLayer::StrokeWeight(const float strokeWeight) { m_GraphicsLayerImpl.StrokeWeight(strokeWeight); }

	void OffscreenGraphicsLayer::NoFill() { m_GraphicsLayerImpl.NoFill(); }
	void OffscreenGraphicsLayer::NoStroke() { m_GraphicsLayerImpl.NoStroke(); }

	void OffscreenGraphicsLayer::SetBlendMode(const Blending::BlendMode& blendMode) { m_GraphicsLayerImpl.SetBlendMode(blendMode); }
	void OffscreenGraphicsLayer::SetRectMode(const RectMode& rectMode) { m_GraphicsLayerImpl.SetRectMode(rectMode); }
	void OffscreenGraphicsLayer::SetImageMode(const RectMode& imageMode) { m_GraphicsLayerImpl.SetImageMode(imageMode); }
	void OffscreenGraphicsLayer::SetEllipseMode(const EllipseMode& ellipseMode) { m_GraphicsLayerImpl.SetEllipseMode(ellipseMode); }
	void OffscreenGraphicsLayer::SetSegmentCountMode(const SegmentCountMode& segmentCountMode) { m_GraphicsLayerImpl.SetSegmentCountMode(segmentCountMode); }

	void OffscreenGraphicsLayer::SetImageTint(const Renderer::Color tint) { m_GraphicsLayerImpl.SetImageTint(tint); }
	void OffscreenGraphicsLayer::SetImageAlpha(const uint8_t alpha) { m_GraphicsLayerImpl.SetImageAlpha(alpha); }
	void OffscreenGraphicsLayer::SetImageOpacity(const float opacity) { m_GraphicsLayerImpl.SetImageOpacity(opacity); }

	void OffscreenGraphicsLayer::Background(const Renderer::Color color) { m_GraphicsLayerImpl.Background(color); }
	void OffscreenGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Rect(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Ellipse(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Point(const float x, const float y) { m_GraphicsLayerImpl.Point(x, y); }
	void OffscreenGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Line(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { m_GraphicsLayerImpl.Triangle(x1, y1, x2, y2, x3, y3); }
	void OffscreenGraphicsLayer::Image(const Texture::Texture& texture, const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Image(texture, x1, y1, x2, y2); }

	OffscreenGraphicsLayer::OffscreenGraphicsLayer(
		const Math::Uint2 viewportSize,
		RendererFacade& renderer,
		Blending::BlendModeActivator& blendModeActivator
	) :	m_RenderTarget(Renderer::OffscreenRenderTarget::Create(viewportSize)),
		m_GraphicsLayerImpl(renderer, viewportSize, blendModeActivator, std::make_unique<IncrementalDepthProvider>(0.0f, 1.0f / 20'000.0f))
	{
	}
}
