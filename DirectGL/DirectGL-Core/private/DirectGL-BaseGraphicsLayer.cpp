module DirectGL;

import :BaseGraphicsLayer;

namespace DGL
{
	void BaseGraphicsLayer::BeginDraw()
	{
		m_RenderStates.Clear();
	}

	void BaseGraphicsLayer::EndDraw()
	{
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

	void BaseGraphicsLayer::Blend(const BlendMode& blendMode)
	{
		PeekState().BlendMode = blendMode;
	}

	void BaseGraphicsLayer::Background(const Color color)
	{
		// Generate vertices for a rectangle covering the entire viewport
		const auto vertices = m_ShapeFactory->GetFilledRectangle(m_Viewport);

		// Render the rectangle with the specified background color
		m_SolidFillBrush->SetColor(color);
		m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
		m_Renderer->Render(vertices, BlendModes::Alpha);
	}

	void BaseGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Compute the boundary of the rectangle
		const auto boundary = Math::FloatBoundary::FromLTWH(x1, y1, x2, y2);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled rectangle.
			const auto vertices = m_ShapeFactory->GetFilledRectangle(boundary);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined rectangle.
			const auto vertices = m_ShapeFactory->GetOutlinedRectangle(boundary, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Compute the center and radius of the ellipse
		const auto radius = Math::Radius::Elliptical(x2 * 0.5f, y2 * 0.5f);
		const auto center = Math::Float2{ x1, y1 };

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled ellipse.
			const auto vertices = m_ShapeFactory->GetFilledEllipse(center, radius, 64);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined ellipse.
			const auto vertices = m_ShapeFactory->GetOutlinedEllipse(center, radius, 64, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Point(const float x, const float y)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for a point rendered as a small filled circle.
			const auto radius = Math::Radius::Circular(state.StrokeWeight * 0.5f);
			const auto center = Math::Float2{ x, y };
			const auto vertices = m_ShapeFactory->GetFilledEllipse(center, radius, 16);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for a line with the specified stroke weight.
			const auto vertices = m_ShapeFactory->GetLine(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void BaseGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled triangle.
			const auto vertices = m_ShapeFactory->GetFilledTriangle(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, Math::Float2{ x3, y3 });

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// TODO(Felix): Implement outlined triangle rendering.
	}

	BaseGraphicsLayer::BaseGraphicsLayer(const Math::Uint2 viewportSize, Renderer::Renderer& renderer, Renderer::ShapeFactory& shapeFactory) :
		m_Renderer(&renderer),
		m_ShapeFactory(&shapeFactory),
		m_SolidFillBrush(Renderer::SolidColorBrush::Create(Colors::White)),
		m_SolidStrokeBrush(Renderer::SolidColorBrush::Create(Colors::White)),
		m_Viewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(viewportSize.X), static_cast<float>(viewportSize.Y))),
		m_ProjectionMatrix(Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f))
	{
	}
}
