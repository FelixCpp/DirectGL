module;

#include <glad/gl.h>

module DirectGL;

import :OffscreenRenderTarget;

namespace DGL
{
	OffscreenRenderTarget::OffscreenRenderTarget(const Math::FloatBoundary& viewport):
		m_Framebuffer(0),
		m_Renderbuffer(0),
		m_TextureId(0),
		m_Viewport(viewport)
	{
		glCreateTextures(GL_TEXTURE, 1, &m_TextureId);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureStorage2D(m_TextureId, 1, GL_RGBA8, static_cast<GLsizei>(m_Viewport.Width), static_cast<GLsizei>(m_Viewport.Height));
		glTextureSubImage2D(m_TextureId, 0, 0, 0, static_cast<GLsizei>(m_Viewport.Width), static_cast<GLsizei>(m_Viewport.Height), GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glCreateRenderbuffers(1, &m_Renderbuffer);
		glRenderbufferStorage(m_Renderbuffer, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(m_Viewport.Width), static_cast<GLsizei>(m_Viewport.Height));

		glCreateFramebuffers(1, &m_Framebuffer);
		glNamedFramebufferRenderbuffer(m_Framebuffer, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);
		glNamedFramebufferTexture(m_Framebuffer, GL_COLOR_ATTACHMENT0, m_TextureId, 0);
		glNamedFramebufferDrawBuffer(m_Framebuffer, GL_COLOR_ATTACHMENT0);

		if (glCheckNamedFramebufferStatus(m_Framebuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Error("Failed to create offscreen render target framebuffer");
		}
	}

	OffscreenRenderTarget::~OffscreenRenderTarget()
	{
		glDeleteFramebuffers(1, &m_Framebuffer);
		glDeleteRenderbuffers(1, &m_Renderbuffer);
		glDeleteTextures(1, &m_TextureId);
	}

	void OffscreenRenderTarget::Activate()
	{
		glViewport(static_cast<GLsizei>(0), static_cast<GLsizei>(0), static_cast<GLsizei>(m_Viewport.Width), static_cast<GLsizei>(m_Viewport.Height));
		glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	}
}