module;

#include <glad/gl.h>

#include <memory>

module DGL;

namespace DGL
{
	std::unique_ptr<OpenGLOffscreenRenderTarget> OpenGLOffscreenRenderTarget::Create(const uint32_t width, const uint32_t height, Renderer& renderer)
	{
		auto texture = OpenGLTexture::Create(width, height, 4, nullptr);
		if (texture == nullptr)
		{
			Error("Failed to create texture for offscreen render target");
			return nullptr;
		}

		GLuint renderbufferId = 0;
		glCreateRenderbuffers(1, &renderbufferId);
		glNamedRenderbufferStorage(renderbufferId, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(width), static_cast<GLsizei>(height));

		GLuint framebufferId = 0;
		glCreateFramebuffers(1, &framebufferId);
		glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);
		glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, texture.get()->GetTextureId(), 0);
		glNamedFramebufferDrawBuffer(framebufferId, GL_COLOR_ATTACHMENT0);

		if (glCheckNamedFramebufferStatus(framebufferId, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Error("Failed to create framebuffer for offscreen render target");
			glDeleteFramebuffers(1, &framebufferId);
			glDeleteRenderbuffers(1, &renderbufferId);
			return nullptr;
		}

		return std::unique_ptr<OpenGLOffscreenRenderTarget>(new OpenGLOffscreenRenderTarget(framebufferId, renderbufferId, std::move(texture), renderer));
	}

	Texture* OpenGLOffscreenRenderTarget::GetTexture() const
	{
		return m_Texture.get();
	}

	Math::Uint2 OpenGLOffscreenRenderTarget::GetSize() const
	{
		return m_Texture->GetSize();
	}

	void OpenGLOffscreenRenderTarget::Begin()
	{
		const auto& [width, height] = GetSize();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}

	void OpenGLOffscreenRenderTarget::End()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void OpenGLOffscreenRenderTarget::DrawRectangle(const Math::FloatBoundary& boundary, const float thickness, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateOutlinedRectangle(boundary, thickness);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLOffscreenRenderTarget::FillRectangle(const Math::FloatBoundary& boundary, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledRectangle(boundary);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLOffscreenRenderTarget::DrawEllipse(const Math::Float2& center, const Radius radius, const float thickness, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateOutlinedEllipse(center, radius, 64, thickness);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLOffscreenRenderTarget::FillEllipse(const Math::Float2& center, const Radius radius, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledEllipse(center, radius, 64);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	void OpenGLOffscreenRenderTarget::Line(const Math::Float2& start, const Math::Float2& end, const float thickness, Brush& brush, const LineCap startCap, const LineCap endCap)
	{
		const Geometry geometry = GeometryFactory::CreateLine(start, end, thickness, startCap, endCap);
		brush.Apply(m_Camera);
		m_Renderer->Submit(geometry);
	}

	OpenGLOffscreenRenderTarget::OpenGLOffscreenRenderTarget(const GLuint framebufferId, const GLuint renderbufferId, std::unique_ptr<Texture> texture, Renderer& renderer):
		m_FramebufferId(framebufferId),
		m_RenderbufferId(renderbufferId),
		m_Texture(std::move(texture)),
		m_Renderer(&renderer),
		m_Camera(static_cast<Math::Float2>(m_Texture->GetSize()))
	{
	}
}