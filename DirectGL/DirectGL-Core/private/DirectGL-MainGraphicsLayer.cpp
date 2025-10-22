module;

#include <memory>

module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	std::unique_ptr<MainGraphicsLayer> MainGraphicsLayer::Create(
		const Math::Uint2 viewportSize,
		RendererFacade& renderer,
		Blending::BlendModeActivator& blendModeActivator
	) {
		return std::unique_ptr<MainGraphicsLayer>(new MainGraphicsLayer(viewportSize, renderer, blendModeActivator));
	}

	void MainGraphicsLayer::Resize(const Math::Uint2 viewportSize)
	{
		m_MainRenderTarget->SetViewport(Math::UintBoundary::FromLTWH(0, 0, viewportSize.X, viewportSize.Y));
		m_GraphicsLayer.SetViewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(viewportSize.X), static_cast<float>(viewportSize.Y)));
	}

	void MainGraphicsLayer::BeginDraw()
	{
		m_GraphicsLayer.BeginDraw();
		m_MainRenderTarget->Activate();
	}

	void MainGraphicsLayer::EndDraw()
	{
		m_GraphicsLayer.EndDraw();
	}

	void MainGraphicsLayer::Resume()
	{
		m_MainRenderTarget->Activate();
	}

	void MainGraphicsLayer::Suspend()
	{
		// Nothing to do for now
	}

	void MainGraphicsLayer::PushState() { m_GraphicsLayer.PushState(); }
	void MainGraphicsLayer::PopState() { m_GraphicsLayer.PopState(); }
	RenderState& MainGraphicsLayer::PeekState() { return m_GraphicsLayer.PeekState(); }

	void MainGraphicsLayer::PushTransform() { m_GraphicsLayer.PushTransform(); }
	void MainGraphicsLayer::PopTransform() { m_GraphicsLayer.PopTransform(); }
	Math::Matrix4x4& MainGraphicsLayer::PeekTransform() { return m_GraphicsLayer.PeekTransform(); }

	void MainGraphicsLayer::ResetTransform() { m_GraphicsLayer.ResetTransform(); }
	void MainGraphicsLayer::Translate(const float x, const float y) { m_GraphicsLayer.Translate(x, y); }
	void MainGraphicsLayer::Scale(const float x, const float y) { m_GraphicsLayer.Scale(x, y); }
	void MainGraphicsLayer::Rotate(const Math::Angle angle) { m_GraphicsLayer.Rotate(angle); }
	void MainGraphicsLayer::Skew(const Math::Angle angleX, const Math::Angle angleY) { m_GraphicsLayer.Skew(angleX, angleY); }

	void MainGraphicsLayer::Fill(const Renderer::Color color) { m_GraphicsLayer.Fill(color); }
	void MainGraphicsLayer::Stroke(const Renderer::Color color) { m_GraphicsLayer.Stroke(color); }
	void MainGraphicsLayer::StrokeWeight(const float strokeWeight) { m_GraphicsLayer.StrokeWeight(strokeWeight); }

	void MainGraphicsLayer::NoFill() { m_GraphicsLayer.NoFill(); }
	void MainGraphicsLayer::NoStroke() { m_GraphicsLayer.NoStroke(); }

	void MainGraphicsLayer::SetBlendMode(const Blending::BlendMode& blendMode) { m_GraphicsLayer.SetBlendMode(blendMode); }
	void MainGraphicsLayer::SetRectMode(const RectMode& rectMode) { m_GraphicsLayer.SetRectMode(rectMode); }
	void MainGraphicsLayer::SetImageMode(const RectMode& imageMode) { m_GraphicsLayer.SetImageMode(imageMode); }
	void MainGraphicsLayer::SetEllipseMode(const EllipseMode& ellipseMode) { m_GraphicsLayer.SetEllipseMode(ellipseMode); }
	void MainGraphicsLayer::SetSegmentCountMode(const SegmentCountMode& segmentCountMode) { m_GraphicsLayer.SetSegmentCountMode(segmentCountMode); }

	void MainGraphicsLayer::Background(const Renderer::Color color) { m_GraphicsLayer.Background(color); }
	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Rect(x1, y1, x2, y2); }
	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Ellipse(x1, y1, x2, y2); }
	void MainGraphicsLayer::Point(const float x, const float y) { m_GraphicsLayer.Point(x, y); }
	void MainGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Line(x1, y1, x2, y2); }
	void MainGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3) { m_GraphicsLayer.Triangle(x1, y1, x2, y2, x3, y3); }
	void MainGraphicsLayer::Image(const Texture::Texture& texture, const float x1, const float y1, const float x2, const float y2) { m_GraphicsLayer.Image(texture, x1, y1, x2, y2); }

	MainGraphicsLayer::MainGraphicsLayer(
		const Math::Uint2 viewportSize,
		RendererFacade& renderer,
		Blending::BlendModeActivator& blendModeActivator
	):	m_MainRenderTarget(
			Renderer::MainRenderTarget::Create(
				Math::UintBoundary::FromLTWH(
					0, 0,
					viewportSize.X, viewportSize.Y
				)
			)
		),
		m_GraphicsLayer(
			renderer,
			viewportSize,
			blendModeActivator,
			std::make_unique<IncrementalDepthProvider>(0.0f, 1.0f / 20'000.0f)
		)
	{
	}
}