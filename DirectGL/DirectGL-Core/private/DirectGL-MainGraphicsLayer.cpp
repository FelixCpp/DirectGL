
module;

#include <glad/gl.h>

module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	std::unique_ptr<MainGraphicsLayer> MainGraphicsLayer::Create(const uint32_t width, const uint32_t height, Renderer::Renderer& renderer)
	{
		return std::unique_ptr<MainGraphicsLayer>(new MainGraphicsLayer(width, height, renderer));
	}

	void MainGraphicsLayer::Resize(const uint32_t width, const uint32_t height)
	{
		m_Viewport = Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f);
		glViewport(0, 0, width, height);
	}

	void MainGraphicsLayer::BeginDraw()
	{
		// Prevent nested BeginDraw calls
		if (m_IsDrawing)
			return;

		m_IsDrawing = true;

		// Query the current framebuffer binding in order to restore it later.
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_CachedFramebufferId);

		// Query the current viewport dimensions in order to restore them later.
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		m_CachedViewport = Math::UintBoundary::FromLTWH(viewport[0], viewport[1], viewport[2], viewport[3]);

		// Bind the offscreen framebuffer for rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_Viewport.Width, m_Viewport.Height);
	}

	void MainGraphicsLayer::EndDraw()
	{
		// Prevent EndDraw calls without a preceding BeginDraw call
		if (not m_IsDrawing)
			return;

		m_IsDrawing = false;

		// Restore the previously bound framebuffer & viewport
		glBindFramebuffer(GL_FRAMEBUFFER, m_CachedFramebufferId);
		glViewport(m_CachedViewport.Left, m_CachedViewport.Top, m_CachedViewport.Width, m_CachedViewport.Height);
	}

	void MainGraphicsLayer::PushState() { m_RenderStates.PushState(); }
	void MainGraphicsLayer::PopState() { m_RenderStates.PopState(); }
	RenderState& MainGraphicsLayer::PeekState() { return m_RenderStates.PeekState(); }

	void MainGraphicsLayer::Fill(const Color color) { PeekState().FillColor = color; PeekState().IsFillEnabled = true; }
	void MainGraphicsLayer::Stroke(const Color color) { PeekState().StrokeColor = color; PeekState().IsStrokeEnabled = true; }
	void MainGraphicsLayer::StrokeWeight(const float strokeWeight) { PeekState().StrokeWeight = strokeWeight; }

	void MainGraphicsLayer::NoFill() { PeekState().IsFillEnabled = false; }
	void MainGraphicsLayer::NoStroke() { PeekState().IsStrokeEnabled = false; }

	void MainGraphicsLayer::Blend(const BlendMode& blendMode) { PeekState().BlendMode = blendMode; }

	void MainGraphicsLayer::Background(const Color color)
	{
		// Generate vertices for a rectangle covering the entire viewport
		const auto vertices = Renderer::GetFilledRectangle(m_Viewport);

		// Render the rectangle with the specified background color
		m_SolidFillBrush->SetColor(color);
		m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
		m_Renderer->Render(vertices, BlendModes::Alpha);
	}

	void MainGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Compute the boundary of the rectangle
		const auto boundary = Math::FloatBoundary::FromLTWH(x1, y1, x2, y2);

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled rectangle.
			const auto vertices = Renderer::GetFilledRectangle(boundary);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined rectangle.
			const auto vertices = Renderer::GetOutlinedRectangle(boundary, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void MainGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
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
			const auto vertices = Renderer::GetFilledEllipse(center, radius, 64);

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for an outlined ellipse.
			const auto vertices = Renderer::GetOutlinedEllipse(center, radius, state.StrokeWeight, 64);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void MainGraphicsLayer::Circle(const float x1, const float y1, const float xy2)
	{
		Ellipse(x1, y1, xy2, xy2);
	}

	void MainGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Only render if the stroke is enabled and the stroke weight is greater than zero
		if (state.IsStrokeEnabled and state.StrokeWeight > 0.0f)
		{
			// Compute the vertices for a line with the specified stroke weight.
			const auto vertices = Renderer::GetLine(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, state.StrokeWeight);

			m_SolidStrokeBrush->SetColor(state.StrokeColor);
			m_SolidStrokeBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}
	}

	void MainGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
	{
		// Get the current render state
		const auto& state = PeekState();

		// Only render if the fill is enabled
		if (state.IsFillEnabled)
		{
			// Compute the vertices for a filled triangle.
			const auto vertices = Renderer::GetFilledTriangle(Math::Float2{ x1, y1 }, Math::Float2{ x2, y2 }, Math::Float2{ x3, y3 });

			m_SolidFillBrush->SetColor(state.FillColor);
			m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
			m_Renderer->Render(vertices, state.BlendMode);
		}

		// TODO(Felix): Implement outlined triangle rendering.
	}

	MainGraphicsLayer::MainGraphicsLayer(const uint32_t width, const uint32_t height, Renderer::Renderer& renderer) :
		m_Renderer(&renderer),
		m_SolidFillBrush(Renderer::SolidColorBrush::Create(Color(255, 255, 255))),
		m_SolidStrokeBrush(Renderer::SolidColorBrush::Create(Color(255, 255, 255))),
		m_CachedFramebufferId(0),
		m_IsDrawing(false),
		m_Viewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))),
		m_ProjectionMatrix(Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f))
	{
	}
}
