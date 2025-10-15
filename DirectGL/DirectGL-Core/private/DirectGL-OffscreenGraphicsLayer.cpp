module;

#include <glad/gl.h>

#include <type_traits>

module DirectGL;

import :OffscreenGraphicsLayer;

namespace DGL
{
	std::unique_ptr<OffscreenGraphicsLayer> OffscreenGraphicsLayer::Create(const Uint2 viewportSize, RendererFacade& renderer, ShapeRenderer::ShapeFactory& shapeFactory)
	{
		return std::unique_ptr<OffscreenGraphicsLayer>(new OffscreenGraphicsLayer(viewportSize, renderer, shapeFactory));
	}

	void OffscreenGraphicsLayer::BeginDraw()
	{
		m_RenderTarget->BeginDraw();
		m_GraphicsLayerImpl.BeginDraw();
	}

	void OffscreenGraphicsLayer::EndDraw()
	{
		m_GraphicsLayerImpl.EndDraw();
		m_RenderTarget->EndDraw();
	}

	void OffscreenGraphicsLayer::PushState() { m_GraphicsLayerImpl.PushState(); }
	void OffscreenGraphicsLayer::PopState() { m_GraphicsLayerImpl.PopState(); }
	RenderState& OffscreenGraphicsLayer::PeekState() { return m_GraphicsLayerImpl.PeekState(); }

	void OffscreenGraphicsLayer::PushTransform() { m_GraphicsLayerImpl.PushTransform(); }
	void OffscreenGraphicsLayer::PopTransform() { m_GraphicsLayerImpl.PopTransform(); }
	Matrix4x4& OffscreenGraphicsLayer::PeekTransform() { return m_GraphicsLayerImpl.PeekTransform(); }

	void OffscreenGraphicsLayer::ResetTransform() { m_GraphicsLayerImpl.ResetTransform(); }
	void OffscreenGraphicsLayer::Translate(const float x, const float y) { m_GraphicsLayerImpl.Translate(x, y); }
	void OffscreenGraphicsLayer::Scale(const float x, const float y) { m_GraphicsLayerImpl.Scale(x, y); }
	void OffscreenGraphicsLayer::Rotate(const Angle angle) { m_GraphicsLayerImpl.Rotate(angle); }
	void OffscreenGraphicsLayer::Skew(const Angle angleX, const Angle angleY) { m_GraphicsLayerImpl.Skew(angleX, angleY); }

	void OffscreenGraphicsLayer::Fill(const Color color) { m_GraphicsLayerImpl.Fill(color); }
	void OffscreenGraphicsLayer::Stroke(const Color color) { m_GraphicsLayerImpl.Stroke(color); }
	void OffscreenGraphicsLayer::StrokeWeight(const float strokeWeight) { m_GraphicsLayerImpl.StrokeWeight(strokeWeight); }

	void OffscreenGraphicsLayer::NoFill() { m_GraphicsLayerImpl.NoFill(); }
	void OffscreenGraphicsLayer::NoStroke() { m_GraphicsLayerImpl.NoStroke(); }

	void OffscreenGraphicsLayer::SetBlendMode(const BlendMode& blendMode) { m_GraphicsLayerImpl.SetBlendMode(blendMode); }
	void OffscreenGraphicsLayer::SetRectMode(const RectMode& rectMode) { m_GraphicsLayerImpl.SetRectMode(rectMode); }
	void OffscreenGraphicsLayer::SetImageMode(const RectMode& imageMode) { m_GraphicsLayerImpl.SetImageMode(imageMode); }
	void OffscreenGraphicsLayer::SetEllipseMode(const EllipseMode& ellipseMode) { m_GraphicsLayerImpl.SetEllipseMode(ellipseMode); }
	void OffscreenGraphicsLayer::SetSegmentCountMode(const SegmentCountMode& segmentCountMode) { m_GraphicsLayerImpl.SetSegmentCountMode(segmentCountMode); }

	void OffscreenGraphicsLayer::Background(const Color color) { m_GraphicsLayerImpl.Background(color); }
	void OffscreenGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Rect(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Ellipse(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Point(const float x, const float y) { m_GraphicsLayerImpl.Point(x, y); }
	void OffscreenGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Line(x1, y1, x2, y2); }
	void OffscreenGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { m_GraphicsLayerImpl.Triangle(x1, y1, x2, y2, x3, y3); }
	void OffscreenGraphicsLayer::Image(const Texture& texture, const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayerImpl.Image(texture, x1, y1, x2, y2); }

	OffscreenGraphicsLayer::OffscreenGraphicsLayer(const Uint2 viewportSize, RendererFacade& renderer, ShapeRenderer::ShapeFactory& shapeFactory) :
		m_RenderTarget(Renderer::OffscreenRenderTarget::Create(viewportSize)),
		m_GraphicsLayerImpl(renderer, shapeFactory)
	{
	}
}
