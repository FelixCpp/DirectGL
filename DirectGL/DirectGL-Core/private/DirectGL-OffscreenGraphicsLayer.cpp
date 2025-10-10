module;

#include <glad/gl.h>

module DirectGL;

import :OffscreenGraphicsLayer;

namespace DGL
{
	std::unique_ptr<OffscreenGraphicsLayer> OffscreenGraphicsLayer::Create(const uint32_t width, const uint32_t height, Renderer::Renderer& renderer)
	{
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		GLuint renderbufferId = 0;
		glCreateRenderbuffers(1, &renderbufferId);
		glNamedRenderbufferStorage(renderbufferId, GL_DEPTH24_STENCIL8, width, height);

		GLuint framebufferId = 0;
		glCreateFramebuffers(1, &framebufferId);
		glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, textureId, 0);
		glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);

		if (glCheckNamedFramebufferStatus(framebufferId, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteTextures(1, &textureId);
			glDeleteRenderbuffers(1, &renderbufferId);
			glDeleteFramebuffers(1, &framebufferId);
			return nullptr;
		}

		return std::unique_ptr<OffscreenGraphicsLayer>(new OffscreenGraphicsLayer(width, height, renderer, framebufferId, renderbufferId, textureId));
	}

	OffscreenGraphicsLayer::~OffscreenGraphicsLayer()
	{
		if (m_FramebufferId != 0) glDeleteFramebuffers(1, &m_FramebufferId);
		if (m_RenderbufferId != 0) glDeleteRenderbuffers(1, &m_RenderbufferId);
		if (m_RenderTextureId != 0) glDeleteTextures(1, &m_RenderTextureId);
	}

	void OffscreenGraphicsLayer::BeginDraw()
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
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, m_Viewport.Width, m_Viewport.Height);
	}

	void OffscreenGraphicsLayer::EndDraw()
	{
		// Prevent EndDraw calls without a preceding BeginDraw call
		if (not m_IsDrawing)
			return;

		m_IsDrawing = false;

		// Restore the previously bound framebuffer & viewport
		glBindFramebuffer(GL_FRAMEBUFFER, m_CachedFramebufferId);
		glViewport(m_CachedViewport.Left, m_CachedViewport.Top, m_CachedViewport.Width, m_CachedViewport.Height);
	}

	void OffscreenGraphicsLayer::PushState() { m_RenderStates.PushState(); }
	void OffscreenGraphicsLayer::PopState() { m_RenderStates.PopState(); }
	RenderState& OffscreenGraphicsLayer::PeekState() { return m_RenderStates.PeekState(); }

	void OffscreenGraphicsLayer::Fill(const Color color) { PeekState().FillColor = color; PeekState().IsFillEnabled = true; }
	void OffscreenGraphicsLayer::Stroke(const Color color) { PeekState().StrokeColor = color; PeekState().IsStrokeEnabled = true; }
	void OffscreenGraphicsLayer::StrokeWeight(const float strokeWeight) { PeekState().StrokeWeight = strokeWeight; }

	void OffscreenGraphicsLayer::NoFill() { PeekState().IsFillEnabled = false; }
	void OffscreenGraphicsLayer::NoStroke() { PeekState().IsStrokeEnabled = false; }

	void OffscreenGraphicsLayer::Blend(const BlendMode& blendMode) { PeekState().BlendMode = blendMode; }

	void OffscreenGraphicsLayer::Background(const Color color)
	{
		// Generate vertices for a rectangle covering the entire viewport
		const auto vertices = Renderer::GetFilledRectangle(m_Viewport);

		// Render the rectangle with the specified background color
		m_SolidFillBrush->SetColor(color);
		m_SolidFillBrush->UploadUniforms(m_ProjectionMatrix);
		m_Renderer->Render(vertices, BlendModes::Alpha);
	}

	void OffscreenGraphicsLayer::Rect(const float x1, const float y1, const float x2, const float y2)
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

	void OffscreenGraphicsLayer::Ellipse(const float x1, const float y1, const float x2, const float y2)
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

	void OffscreenGraphicsLayer::Circle(const float x1, const float y1, const float xy2)
	{
		Ellipse(x1, y1, xy2, xy2);
	}

	void OffscreenGraphicsLayer::Line(const float x1, const float y1, const float x2, const float y2)
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

	void OffscreenGraphicsLayer::Triangle(const float x1, const float y1, const float x2, const float y2, const float x3, const float y3)
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

	OffscreenGraphicsLayer::OffscreenGraphicsLayer(const uint32_t width, const uint32_t height, Renderer::Renderer& renderer, const GLuint framebufferId, const GLuint renderbufferId, const GLuint renderTextureId) :
		m_Renderer(&renderer),
		m_SolidFillBrush(Renderer::SolidColorBrush::Create(Color(255, 255, 255))),
		m_SolidStrokeBrush(Renderer::SolidColorBrush::Create(Color(255, 255, 255))),
		m_FramebufferId(framebufferId),
		m_RenderbufferId(renderbufferId),
		m_RenderTextureId(renderTextureId),
		m_CachedFramebufferId(0),
		m_IsDrawing(false),
		m_Viewport(Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))),
		m_ProjectionMatrix(Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f))
	{
	}
}
