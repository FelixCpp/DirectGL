module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	std::unique_ptr<MainGraphicsLayer> MainGraphicsLayer::Create(
		const Uint2 viewportSize,
		Renderer::Renderer& renderer,
		Renderer::ShapeFactory& shapeFactory
	) {
		return std::unique_ptr<MainGraphicsLayer>(new MainGraphicsLayer(viewportSize, renderer, shapeFactory));
	}

	void MainGraphicsLayer::Resize(const Uint2 viewportSize)
	{
		m_MainRenderTarget->SetViewport(UintBoundary::FromLTWH(0, 0, viewportSize.X, viewportSize.Y));
		m_GraphicsLayer.SetViewport(FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(viewportSize.X), static_cast<float>(viewportSize.Y)));
	}

	void MainGraphicsLayer::BeginDraw()
	{
		m_MainRenderTarget->BeginDraw();
		m_GraphicsLayer.BeginDraw();
	}

	void MainGraphicsLayer::EndDraw()
	{
		m_GraphicsLayer.EndDraw();
		m_MainRenderTarget->EndDraw();
	}

	void MainGraphicsLayer::PushState() { m_GraphicsLayer.PushState(); }
	void MainGraphicsLayer::PopState() { m_GraphicsLayer.PopState(); }
	RenderState& MainGraphicsLayer::PeekState() { return m_GraphicsLayer.PeekState(); }

	void MainGraphicsLayer::PushTransform() { m_GraphicsLayer.PushTransform(); }
	void MainGraphicsLayer::PopTransform() { m_GraphicsLayer.PopTransform(); }
	Matrix4x4& MainGraphicsLayer::PeekTransform() { return m_GraphicsLayer.PeekTransform(); }

	void MainGraphicsLayer::ResetTransform() { m_GraphicsLayer.ResetTransform(); }
	void MainGraphicsLayer::Translate(const float x, const float y) { m_GraphicsLayer.Translate(x, y); }
	void MainGraphicsLayer::Scale(const float x, const float y) { m_GraphicsLayer.Scale(x, y); }
	void MainGraphicsLayer::Rotate(const Angle angle) { m_GraphicsLayer.Rotate(angle); }
	void MainGraphicsLayer::Skew(const Angle angleX, const Angle angleY) { m_GraphicsLayer.Skew(angleX, angleY); }

	void MainGraphicsLayer::Fill(const Color color) { m_GraphicsLayer.Fill(color); }
	void MainGraphicsLayer::Stroke(const Color color) { m_GraphicsLayer.Stroke(color); }
	void MainGraphicsLayer::StrokeWeight(const float strokeWeight) { m_GraphicsLayer.StrokeWeight(strokeWeight); }

	void MainGraphicsLayer::NoFill() { m_GraphicsLayer.NoFill(); }
	void MainGraphicsLayer::NoStroke() { m_GraphicsLayer.NoStroke(); }

	void MainGraphicsLayer::SetBlendMode(const BlendMode& blendMode) { m_GraphicsLayer.SetBlendMode(blendMode); }
	void MainGraphicsLayer::SetRectMode(const DGL::RectMode& rectMode) { m_GraphicsLayer.SetRectMode(rectMode); }
	void MainGraphicsLayer::SetEllipseMode(const DGL::EllipseMode& ellipseMode) { m_GraphicsLayer.SetEllipseMode(ellipseMode); }
	void MainGraphicsLayer::SetSegmentCountMode(const SegmentCountMode& segmentCountMode) { m_GraphicsLayer.SetSegmentCountMode(segmentCountMode); }

	void MainGraphicsLayer::Background(const Color color) { m_GraphicsLayer.Background(color); }
	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Rect(x1, y1, x2, y2); }
	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Ellipse(x1, y1, x2, y2); }
	void MainGraphicsLayer::Point(const float x, const float y) { m_GraphicsLayer.Point(x, y); }
	void MainGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Line(x1, y1, x2, y2); }
	void MainGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { m_GraphicsLayer.Triangle(x1, y1, x2, y2, x3, y3); }
	void MainGraphicsLayer::Image(const Texture& texture, const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Image(texture, x1, y1, x2, y2); }

	MainGraphicsLayer::MainGraphicsLayer(const Uint2 viewportSize, Renderer::Renderer& renderer, Renderer::ShapeFactory& shapeFactory):
		m_MainRenderTarget(Renderer::MainRenderTarget::Create(UintBoundary::FromLTWH(0, 0, viewportSize.X, viewportSize.Y))),
		m_GraphicsLayer(renderer, shapeFactory)
	{
	}
}