module;

#include <array>

module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	MainGraphicsLayer::MainGraphicsLayer(const std::weak_ptr<MeshRenderer>& meshRenderer, const Math::FloatBoundary& viewport):
		m_MeshRenderer(meshRenderer),
		m_DepthProvider(0.0f, 1.0f / 20'000.0f),
		m_Viewport(viewport),
		m_ProjectionMatrix(Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f))
	{
	}

	void MainGraphicsLayer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	const Math::FloatBoundary& MainGraphicsLayer::GetViewport() const
	{
		return m_Viewport;
	}

	void MainGraphicsLayer::BeginDraw()
	{
		m_RenderStyleStack.Reset();
		m_MeshRenderer.lock()->BeginDraw(m_ProjectionMatrix);
	}

	void MainGraphicsLayer::EndDraw()
	{
		m_MeshRenderer.lock()->EndDraw();
	}

	void MainGraphicsLayer::PushStyle(const bool extendCurrentStyle)
	{
		m_RenderStyleStack.PushStyle(extendCurrentStyle ? PeekStyle() : RenderStyle());
	}

	void MainGraphicsLayer::PopStyle()
	{
		m_RenderStyleStack.PopStyle();
	}

	RenderStyle& MainGraphicsLayer::PeekStyle()
	{
		return m_RenderStyleStack.PeekStyle();
	}

	void MainGraphicsLayer::SetRectMode(const RectMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.RectMode = mode;
	}

	void MainGraphicsLayer::SetEllipseMode(const EllipseMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.EllipseMode = mode;
	}

	void MainGraphicsLayer::SetEllipseSegmentsMode(const EllipseSegmentsMode& mode)
	{
		RenderStyle& style = PeekStyle();
		style.EllipseSegmentsMode = mode;
	}

	void MainGraphicsLayer::SetFillColor(const color_t color)
	{
		RenderStyle& style = PeekStyle();
		style.FillColor = color;
		style.IsFillEnabled = true;
	}

	void MainGraphicsLayer::SetFillColorDisabled()
	{
		RenderStyle& style = PeekStyle();
		style.IsFillEnabled = false;
	}

	void MainGraphicsLayer::SetStrokeColor(const color_t color)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeColor = color;
		style.IsStrokeEnabled = true;
	}

	void MainGraphicsLayer::SetStrokeColorDisabled()
	{
		RenderStyle& style = PeekStyle();
		style.IsStrokeEnabled = false;
	}

	void MainGraphicsLayer::SetStrokeWeight(const float weight)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeWeight = weight;
	}

	void MainGraphicsLayer::SetStrokeJoin(const StrokeJoin joinStyle)
	{
		RenderStyle& style = PeekStyle();
		style.JoinStyle = joinStyle;
	}

	void MainGraphicsLayer::SetStrokeCap(const StrokeCap strokeCap)
	{
		RenderStyle& style = PeekStyle();
		style.StrokeCap = strokeCap;
	}

	void MainGraphicsLayer::BeginShape(ShapeMode mode)
	{

	}

	void MainGraphicsLayer::EndShape(ShapeClosingMode mode)
	{

	}

	void MainGraphicsLayer::Vertex(float x, float y)
	{

	}

	void MainGraphicsLayer::Background(const color_t color)
	{
		const auto [left, top, width, height] = m_Viewport;
		const float right = left + width;
		const float bottom = top + height;

		const Mesh backgroundMesh = MeshBuilder::GenerateQuadMesh(
			std::array {
				Math::Float2 { left, top },
				Math::Float2 { right, top },
				Math::Float2 { right, bottom },
				Math::Float2 { left, bottom }
			},
			std::array { color, color, color, color },
			GetCurrentDepth()
		);

		Render(backgroundMesh);
	}

	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();
		const Math::FloatBoundary boundary = style.RectMode(x1, y1, x2, y2);

		const Math::Float2 topLeft = boundary.TopLeft();
		const Math::Float2 topRight = boundary.TopRight();
		const Math::Float2 bottomRight = boundary.BottomRight();
		const Math::Float2 bottomLeft = boundary.BottomLeft();
		const std::array corners = { topLeft, topRight, bottomRight, bottomLeft };

		if (style.IsFillEnabled)
		{
			const Mesh quadMesh = MeshBuilder::GenerateQuadMesh(
				corners,
				std::array { style.FillColor, style.FillColor, style.FillColor, style.FillColor },
				GetCurrentDepth()
			);

			Render(quadMesh);
		}

		if (style.IsStrokeEnabled)
		{
			const Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
				corners,
				std::array { style.StrokeColor, style.StrokeColor, style.StrokeColor, style.StrokeColor },
				style.StrokeWeight, style.JoinStyle, style.StrokeCap, true, // Should close stroke
				GetCurrentDepth()
			);

			Render(outlinedMesh);
		}
	}

	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();
		const Math::FloatBoundary boundary = style.EllipseMode(x1, y1, x2, y2);
		const Math::Radius radius = Math::Radius::Elliptical(boundary.Width / 2.0f, boundary.Height / 2.0f);

		if (style.IsFillEnabled)
		{
			const size_t segments = style.EllipseSegmentsMode(radius, Math::Degrees(360.0f));
			const Mesh ellipseMesh = MeshBuilder::GenerateEllipseMesh(boundary.Center(), style.FillColor, radius, segments, GetCurrentDepth());
			Render(ellipseMesh);
		}

		if (style.IsStrokeEnabled)
		{
			// TODO(Felix): Implement stroked ellipse
		}
	}

	void MainGraphicsLayer::Point(float x, float y)
	{
		const RenderStyle& style = PeekStyle();
		const size_t segments = style.EllipseSegmentsMode(Math::Radius::Circular(style.StrokeWeight), Math::Degrees(360.0f));

		const Mesh pointMesh = MeshBuilder::GenerateEllipseMesh(
			{ x, y },
			style.StrokeColor,
			Math::Radius::Circular(style.StrokeWeight),
			segments,
			GetCurrentDepth()
		);

		Render(pointMesh);
	}

	void MainGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();
		const size_t segments = style.EllipseSegmentsMode(Math::Radius::Circular(style.StrokeWeight), Math::Degrees(180.0f));

		const Math::Float2 start = { x1, y1 };
		const Math::Float2 end = { x2, y2 };

		const Mesh lineMesh = MeshBuilder::GenerateLineMesh(
			std::array { start, end },
			std::array { style.StrokeColor, style.StrokeColor },
			style.StrokeCap,
			style.StrokeWeight,
			segments,
			GetCurrentDepth()
		);

		Render(lineMesh);
	}

	void MainGraphicsLayer::Render(const Mesh& mesh)
	{
		if (const auto renderer = m_MeshRenderer.lock())
		{
			renderer->Submit(mesh);
		}
	}

	float MainGraphicsLayer::GetCurrentDepth()
	{
		return m_DepthProvider.GetAndIncrement();
	}
}