module DirectGL;

import :BaseGraphicsLayer;

namespace DGL
{
	void BaseGraphicsLayer::SetViewport(const FloatBoundary viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f);
	}

	const FloatBoundary& BaseGraphicsLayer::GetViewport() const
	{
		return m_Viewport;
	}

	void BaseGraphicsLayer::BeginDraw()
	{
		m_RenderStates.Clear();
		ResetTransform();
	}

	void BaseGraphicsLayer::EndDraw()
	{
		// Nothing to do for now.
	}

	void BaseGraphicsLayer::PushState()
	{
		m_RenderStates.PushState();
	}

	void BaseGraphicsLayer::PopState()
	{
		m_RenderStates.PopState();
	}

	RenderState& BaseGraphicsLayer::PeekState()
	{
		return m_RenderStates.PeekState();
	}

	void BaseGraphicsLayer::PushTransform()
	{
		PeekState().TransformationStack.PushTransform();
	}

	void BaseGraphicsLayer::PopTransform()
	{
		PeekState().TransformationStack.PopTransform();
	}

	Matrix4x4& BaseGraphicsLayer::PeekTransform()
	{
		return PeekState().TransformationStack.PeekTransform();
	}

	void BaseGraphicsLayer::ResetTransform()
	{
		PeekTransform() = Matrix4x4::Identity;
	}

	void BaseGraphicsLayer::Translate(const float x, const float y)
	{
		PeekTransform() *= Matrix4x4::Translation(x, y, 0.0f);
	}

	void BaseGraphicsLayer::Scale(const float x, const float y)
	{
		PeekTransform() *= Matrix4x4::Scaling(x, y, 1.0f);
	}

	void BaseGraphicsLayer::Rotate(const Angle angle)
	{
		PeekTransform() *= Matrix4x4::Rotation(angle);
	}

	void BaseGraphicsLayer::Skew(const Angle angleX, const Angle angleY)
	{
		PeekTransform() *= Matrix4x4::Skew(angleX, angleY);
	}

	void BaseGraphicsLayer::Fill(const Color color)
	{
		RenderState& state = PeekState();
		state.FillColor = color;
		state.IsFillEnabled = true;
	}

	void BaseGraphicsLayer::Stroke(const Color color)
	{
		RenderState& state = PeekState();
		state.StrokeColor = color;
		state.IsStrokeEnabled = true;
	}

	void BaseGraphicsLayer::StrokeWeight(const float strokeWeight)
	{
		PeekState().StrokeWeight = strokeWeight;
	}

	void BaseGraphicsLayer::NoFill()
	{
		PeekState().IsFillEnabled = false;
	}

	void BaseGraphicsLayer::NoStroke()
	{
		PeekState().IsStrokeEnabled = false;
	}

	void BaseGraphicsLayer::SetBlendMode(const BlendMode& blendMode)
	{
		PeekState().BlendMode = blendMode;
	}

	void BaseGraphicsLayer::RectMode(const DGL::RectMode& rectMode)
	{
		PeekState().RectMode = rectMode;
	}

	void BaseGraphicsLayer::EllipseMode(const DGL::EllipseMode& ellipseMode)
	{
		PeekState().EllipseMode = ellipseMode;
	}

	void BaseGraphicsLayer::Background(const Color color)
	{
		// Generate vertices for a rectangle covering the entire viewport
		const auto vertices = m_ShapeFactory->GetFilledRectangle(m_Viewport);

		// Render the rectangle with the specified background color
		m_SolidFillBrush->SetColor(color);
		m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, Matrix4x4::Identity);
		m_Renderer->Render(vertices, BlendModes::Alpha);
	}

	void BaseGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Compute the boundary of the rectangle
		const auto boundary = state.RectMode(x1, y1, x2, y2);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled rectangle.
			const auto vertices = m_ShapeFactory->GetFilledRectangle(boundary);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined rectangle.
			const auto vertices = m_ShapeFactory->GetOutlinedRectangle(boundary, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Compute the boundary of the ellipse
		const auto boundary = state.EllipseMode(x1, y1, x2, y2);

		// Compute the center and radius of the ellipse
		const auto center = boundary.Center();
		const auto radius = Radius::Elliptical(boundary.Width * 0.5f, boundary.Height * 0.5f);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled ellipse.
			const auto vertices = m_ShapeFactory->GetFilledEllipse(center, radius, 64);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined ellipse.
			const auto vertices = m_ShapeFactory->GetOutlinedEllipse(center, radius, 64, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Point(const float x, const float y)
	{
		// Get the current render state
		auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the boundary of the point rendered as a small filled circle.
			const auto boundary = state.EllipseMode(x, y, state.StrokeWeight, state.StrokeWeight);

			// Compute the vertices for a point rendered as a small filled circle.
			const auto radius = Radius::Elliptical(boundary.Width * 0.5f, boundary.Height * 0.5f);
			const auto center = boundary.Center();
			const auto vertices = m_ShapeFactory->GetFilledEllipse(center, radius, 16);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for a line with the specified stroke weight.
			const auto vertices = m_ShapeFactory->GetLine(Float2{ x1, y1 }, Float2{ x2, y2 }, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		// Get the current render state
		auto& state = PeekState();

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled triangle.
			const auto vertices = m_ShapeFactory->GetFilledTriangle(Float2{ x1, y1 }, Float2{ x2, y2 }, Float2{ x3, y3 });

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// TODO(Felix): Implement outlined triangle rendering.
	}

	void BaseGraphicsLayer::Image(const Texture& texture, const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Compute the boundary of the image
		const auto boundary = FloatBoundary::FromLTWH(x1, y1, x2, y2);

		// Compute the vertices for a textured rectangle.
		const auto vertices = m_ShapeFactory->GetFilledRectangle(boundary);

		m_TextureFillBrush->SetTexture(&texture);
		m_TextureFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
		m_Renderer->Render(vertices, state.BlendMode);
	}

	BaseGraphicsLayer::BaseGraphicsLayer(Renderer::Renderer& renderer, Renderer::ShapeFactory& shapeFactory) :
		m_Renderer(&renderer),
		m_ShapeFactory(&shapeFactory),
		m_SolidFillBrush(Renderer::SolidColorBrush::Create(Colors::White)),
		m_SolidStrokeBrush(Renderer::SolidColorBrush::Create(Colors::White)),
		m_TextureFillBrush(Renderer::TextureBrush::Create())
	{
	}
}
