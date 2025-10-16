module;

#include <cstdio>

module DirectGL;

import :BaseGraphicsLayer;

namespace DGL
{
	void BaseGraphicsLayer::SetViewport(const Math::FloatBoundary viewport)
	{
		m_Viewport = viewport;
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(m_Viewport, 1.0f, -1.0f);
	}

	const Math::FloatBoundary& BaseGraphicsLayer::GetViewport() const
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

	Math::Matrix4x4& BaseGraphicsLayer::PeekTransform()
	{
		return PeekState().TransformationStack.PeekTransform();
	}

	void BaseGraphicsLayer::ResetTransform()
	{
		PeekTransform() = Math::Matrix4x4::Identity;
	}

	void BaseGraphicsLayer::Translate(const float x, const float y)
	{
		PeekTransform() *= Math::Matrix4x4::Translation(x, y, 0.0f);
	}

	void BaseGraphicsLayer::Scale(const float x, const float y)
	{
		PeekTransform() *= Math::Matrix4x4::Scaling(x, y, 1.0f);
	}

	void BaseGraphicsLayer::Rotate(const Math::Angle angle)
	{
		PeekTransform() *= Math::Matrix4x4::Rotation(angle);
	}

	void BaseGraphicsLayer::Skew(const Math::Angle angleX, const Math::Angle angleY)
	{
		PeekTransform() *= Math::Matrix4x4::Skew(angleX, angleY);
	}

	void BaseGraphicsLayer::Fill(const Renderer::Color color)
	{
		RenderState& state = PeekState();
		state.FillColor = color;
		state.IsFillEnabled = true;
	}

	void BaseGraphicsLayer::Stroke(const Renderer::Color color)
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

	void BaseGraphicsLayer::SetBlendMode(const Blending::BlendMode& blendMode)
	{
		PeekState().BlendMode = blendMode;
	}

	void BaseGraphicsLayer::SetRectMode(const DGL::RectMode& rectMode)
	{
		PeekState().RectMode = rectMode;
	}

	void BaseGraphicsLayer::SetImageMode(const DGL::RectMode& imageMode)
	{
		PeekState().ImageMode = imageMode;
	}

	void BaseGraphicsLayer::SetEllipseMode(const DGL::EllipseMode& ellipseMode)
	{
		PeekState().EllipseMode = ellipseMode;
	}

	void BaseGraphicsLayer::SetSegmentCountMode(const SegmentCountMode& segmentCountMode)
	{
		PeekState().SegmentCountMode = segmentCountMode;
	}

	void BaseGraphicsLayer::Background(const Renderer::Color color)
	{
		// Render the rectangle with the specified background color
		m_BlendModeActivator->Activate(Blending::BlendModes::Opaque);
		m_SolidFillBrush->SetColor(color);
		m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, Math::Matrix4x4::Identity);
		m_Renderer->FillRectangle(m_Viewport);
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
			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->FillRectangle(boundary);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->DrawRectangle(boundary, state.StrokeWeight);
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
		const auto radius = Math::Radius::Elliptical(boundary.Width * 0.5f, boundary.Height * 0.5f);
		const auto segments = state.SegmentCountMode(radius);
		if (segments <= 0) return;

		m_BlendModeActivator->Activate(state.BlendMode);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->FillEllipse(center, radius, segments);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->DrawEllipse(center, radius, segments, state.StrokeWeight);
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
			const auto radius = Math::Radius::Elliptical(boundary.Width * 0.5f, boundary.Height * 0.5f);
			const auto center = boundary.Center();
			const auto segments = state.SegmentCountMode(radius);

			m_BlendModeActivator->Activate(state.BlendMode);
			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->FillEllipse(center, radius, segments);
		}
	}

	void BaseGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			m_BlendModeActivator->Activate(state.BlendMode);
			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->Line({ x1, y1 }, { x2, y2 }, state.StrokeWeight);
		}
	}

	void BaseGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		// Get the current render state
		auto& state = PeekState();

		m_BlendModeActivator->Activate(state.BlendMode);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
			m_Renderer->FillTriangle(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, Math::Float2{ x3, y3 });
		}

		// TODO(Felix): Implement outlined triangle rendering.
	}

	void BaseGraphicsLayer::Image(const Texture::Texture& texture, const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		auto& state = PeekState();

		// Compute the boundary of the image
		const auto boundary = state.ImageMode(x1, y1, x2, y2);

		m_BlendModeActivator->Activate(state.BlendMode);
		m_TextureFillBrush->SetTexture(&texture);
		m_TextureFillBrush->UploadUniforms(m_ProjectionMatrix, state.TransformationStack.PeekTransform());
		m_Renderer->Image(boundary);
	}

	BaseGraphicsLayer::BaseGraphicsLayer(RendererFacade& renderer, ShapeRenderer::ShapeFactory& shapeFactory, Blending::BlendModeActivator& blendModeActivator) :
		m_Renderer(&renderer),
		m_ShapeFactory(&shapeFactory),
		m_BlendModeActivator(&blendModeActivator),
		m_SolidFillBrush(Brushes::SolidColorBrush::Create(Colors::White)),
		m_SolidStrokeBrush(Brushes::SolidColorBrush::Create(Colors::White)),
		m_TextureFillBrush(Brushes::TextureBrush::Create())
	{
	}
}
