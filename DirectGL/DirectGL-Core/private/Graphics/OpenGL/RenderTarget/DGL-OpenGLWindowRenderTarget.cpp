module;

#include <glad/gl.h>

module DGL;

namespace DGL
{
	std::unique_ptr<OpenGLWindowRenderTarget> OpenGLWindowRenderTarget::Create(System::Window& window, Renderer& renderer)
	{
		return std::unique_ptr<OpenGLWindowRenderTarget>(new OpenGLWindowRenderTarget(window, renderer));
	}

	void OpenGLWindowRenderTarget::Resize(const uint32_t width, const uint32_t height)
	{
		m_Camera.SetSize({ static_cast<float>(width), static_cast<float>(height) });
	}

	Math::Uint2 OpenGLWindowRenderTarget::GetSize() const
	{
		return static_cast<Math::Uint2>(m_Camera.GetSize());
	}

	System::Window& OpenGLWindowRenderTarget::GetTarget() const
	{
		return *m_Window;
	}

	void OpenGLWindowRenderTarget::Begin()
	{
		const auto& [width, height] = m_Camera.GetSize();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLWindowRenderTarget::End()
	{
		// Nothing to do here.
	}

	void OpenGLWindowRenderTarget::DrawRectangle(const Math::FloatBoundary& boundary, const float thickness, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateOutlinedRectangle(boundary, thickness);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLWindowRenderTarget::FillRectangle(const Math::FloatBoundary& boundary, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledRectangle(boundary);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLWindowRenderTarget::DrawEllipse(const Math::Float2& center, const Radius radius, const float thickness, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateOutlinedEllipse(center, radius, 64, thickness);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLWindowRenderTarget::FillEllipse(const Math::Float2& center, const Radius radius, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledEllipse(center, radius, 64);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLWindowRenderTarget::Line(const Math::Float2& start, const Math::Float2& end, const float thickness, Brush& brush, const LineCap startCap, const LineCap endCap)
	{
		const Geometry geometry = GeometryFactory::CreateLine(start, end, thickness, startCap, endCap);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	OpenGLWindowRenderTarget::OpenGLWindowRenderTarget(System::Window& window, Renderer& renderer) :
		m_Window(&window),
		m_Renderer(&renderer),
		m_Camera(static_cast<Math::Float2>(window.GetSize()))
	{
	}
}