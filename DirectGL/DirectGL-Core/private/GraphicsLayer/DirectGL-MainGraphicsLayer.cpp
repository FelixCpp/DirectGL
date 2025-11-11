module;

#include <array>

module DirectGL;

import :MainGraphicsLayer;
import :MeshBuilder;

namespace DGL
{
	MainGraphicsLayer::MainGraphicsLayer(const std::weak_ptr<MeshRenderer>& meshRenderer, const Math::FloatBoundary& viewport):
		m_MeshRenderer(meshRenderer),
		m_DepthProvider(-1.0f, 1.0f / 20'000.0f),
		m_RenderTarget(viewport),
		m_ProjectionMatrix(Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f))
	{
	}

	void MainGraphicsLayer::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_RenderTarget.SetViewport(viewport);
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(viewport, -1.0f, 1.0f);
	}

	const Math::FloatBoundary& MainGraphicsLayer::GetViewport() const
	{
		return m_RenderTarget.GetViewport();
	}

	void MainGraphicsLayer::BeginDraw()
	{
		m_RenderStyleStack.Reset();
		m_DepthProvider.Reset();
		m_MeshRenderer.lock()->BeginDraw(m_ProjectionMatrix);
		m_RenderTarget.Activate();
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

	void MainGraphicsLayer::PushMatrix(const bool extendCurrentMatrix)
	{
		PeekStyle().MatrixStack.PushMatrix(extendCurrentMatrix);
	}

	void MainGraphicsLayer::PopMatrix()
	{
		PeekStyle().MatrixStack.PopMatrix();
	}

	Math::Matrix4x4& MainGraphicsLayer::PeekMatrix()
	{
		return PeekStyle().MatrixStack.PeekMatrix();
	}

	void MainGraphicsLayer::Translate(const float x, const float y)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Translation(x, y, 0.0f);
	}

	void MainGraphicsLayer::Rotate(const Math::Angle angle)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Rotation(angle);
	}

	void MainGraphicsLayer::Scale(const float scaleX, const float scaleY)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Scaling(scaleX, scaleY, 1.0f);
	}

	void MainGraphicsLayer::Shear(const Math::Angle shearX, const Math::Angle shearY)
	{
		Math::Matrix4x4& matrix = PeekMatrix();
		matrix = matrix * Math::Matrix4x4::Skew(shearX, shearY);
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

	void MainGraphicsLayer::SetBlendMode(const BlendMode& blendMode)
	{
		RenderStyle& style = PeekStyle();
		style.BlendMode = blendMode;
	}

	void MainGraphicsLayer::BeginShape(const ShapeMode mode)
	{
	}

	void MainGraphicsLayer::EndShape(const ShapeClosingMode mode)
	{
	}

	void MainGraphicsLayer::Vertex(float x, float y)
	{
	}

	void MainGraphicsLayer::Background(const color_t color)
	{
		const auto [left, top, width, height] = GetViewport();
		const float right = left + width;
		const float bottom = top + height;

		const float depth = GetCurrentDepth();
		const std::array backgroundPoints = MeshBuilder::GenerateQuadPoints(
			{ left, top },
			{ right, top },
			{ right, bottom },
			{ left, bottom }
		);

		const Mesh backgroundMesh = MeshBuilder::GenerateFilledQuadMesh(
			backgroundPoints,
			std::array { color, color, color, color },
			depth
		);

		Render(backgroundMesh, BlendMode::Opaque, PeekMatrix());
	}

	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		const Math::FloatBoundary boundary = style.RectMode(x1, y1, x2, y2);

		const Math::Float2 topLeft = boundary.TopLeft();
		const Math::Float2 topRight = boundary.TopRight();
		const Math::Float2 bottomRight = boundary.BottomRight();
		const Math::Float2 bottomLeft = boundary.BottomLeft();
		const std::array rectPoints = MeshBuilder::GenerateQuadPoints(topLeft, topRight, bottomRight, bottomLeft);

		if (style.IsFillEnabled)
		{
			const Mesh quadMesh = MeshBuilder::GenerateFilledQuadMesh(
				rectPoints,
				std::array { style.FillColor, style.FillColor, style.FillColor, style.FillColor },
				GetCurrentDepth()
			);

			Render(quadMesh, style.BlendMode, PeekMatrix());
		}

		if (style.IsStrokeEnabled)
		{
			const Mesh outlinedRectMesh = MeshBuilder::GenerateOutlinedMesh(
				rectPoints,
				std::array { style.StrokeColor, style.StrokeColor, style.StrokeColor, style.StrokeColor }, 
				style.StrokeWeight, style.JoinStyle, 32, GetCurrentDepth()
			);

			Render(outlinedRectMesh, style.BlendMode, PeekMatrix());
		}
	}

	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		const Math::FloatBoundary boundary = style.EllipseMode(x1, y1, x2, y2);
		const Math::Radius radius = Math::Radius::Elliptical(boundary.Width / 2.0f, boundary.Height / 2.0f);
		const Math::Float2 center = boundary.Center();
		const size_t segmentCount = style.EllipseSegmentsMode(radius, Math::Degrees(360.0f));
		const std::vector<Math::Float2> ellipsePoints = MeshBuilder::GenerateEllipsePoints(center, radius, segmentCount);

		if (style.IsFillEnabled)
		{
			const std::vector ellipseColors(segmentCount, style.FillColor);
			const Mesh filledEllipseMesh = MeshBuilder::GenerateFilledEllipseMesh(center, ellipsePoints, ellipseColors, GetCurrentDepth());
			Render(filledEllipseMesh, style.BlendMode, PeekMatrix());
		}

		if (style.IsStrokeEnabled)
		{
			const std::vector ellipseColors(segmentCount, style.StrokeColor);
			const Mesh outlinedEllipseMesh = MeshBuilder::GenerateOutlinedMesh(ellipsePoints, ellipseColors, style.StrokeWeight, style.JoinStyle, 32, GetCurrentDepth());
			Render(outlinedEllipseMesh, style.BlendMode, PeekMatrix());
		}
	}

	void MainGraphicsLayer::Point(float x, float y)
	{
		const RenderStyle& style = PeekStyle();
		const size_t segments = style.EllipseSegmentsMode(Math::Radius::Circular(style.StrokeWeight), Math::Degrees(360.0f));
		const std::vector ellipsePoints = MeshBuilder::GenerateEllipsePoints({ x, y }, Math::Radius::Circular(style.StrokeWeight / 2.0f), segments);
		const std::vector ellipseColors(segments, style.StrokeColor);

		const Mesh pointMesh = MeshBuilder::GenerateFilledEllipseMesh({ x, y }, ellipsePoints, ellipseColors, GetCurrentDepth());
		Render(pointMesh, style.BlendMode, PeekMatrix());
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

		Render(lineMesh, style.BlendMode, PeekMatrix());
	}

	void MainGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		const RenderStyle& style = PeekStyle();

		// If there's no fill or stroke, we can skip rendering entirely
		if (not style.IsFillEnabled and not style.IsStrokeEnabled)
		{
			return;
		}

		const std::array trianglePoints = MeshBuilder::GenerateTrianglePoints({ x1, y1 }, { x2, y2 }, { x3, y3 });

		if (style.IsFillEnabled)
		{
			const Mesh filledTriangleMesh = MeshBuilder::GenerateFilledTriangleMesh(
				trianglePoints,
				std::array { style.FillColor, style.FillColor, style.FillColor },
				GetCurrentDepth()
			);

			Render(filledTriangleMesh, style.BlendMode, PeekMatrix());
		}

		if (style.IsStrokeEnabled)
		{
			const Mesh outlinedTriangleMesh = MeshBuilder::GenerateOutlinedMesh(
				trianglePoints,
				std::array { style.StrokeColor, style.StrokeColor, style.StrokeColor },
				style.StrokeWeight,
				style.JoinStyle,
				32,
				GetCurrentDepth()
			);

			Render(outlinedTriangleMesh, style.BlendMode, PeekMatrix());
		}
	}

	void MainGraphicsLayer::Render(const Mesh& mesh, const BlendMode& blendMode, const Math::Matrix4x4& modelMatrix)
	{
		if (const auto renderer = m_MeshRenderer.lock())
		{
			renderer->Submit(mesh, blendMode, modelMatrix);
		}
	}

	float MainGraphicsLayer::GetCurrentDepth()
	{
		return m_DepthProvider.GetAndIncrement();
	}
}