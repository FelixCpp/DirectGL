module;

#include <glad/gl.h>

#include <memory>

module DGL;

namespace DGL
{
	std::unique_ptr<OpenGLFramebuffer> OpenGLFramebuffer::Create(const uint32_t width, const uint32_t height)
	{
		GLuint colorTextureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &colorTextureId);
		if (colorTextureId == 0)
		{
			Error("Failed to create color texture");
			return nullptr;
		}

		glTextureStorage2D(colorTextureId, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(colorTextureId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		GLuint renderbufferId = 0;
		glCreateRenderbuffers(1, &renderbufferId);
		if (renderbufferId == 0)
		{
			Error("Failed to create renderbuffer");
			glDeleteTextures(1, &colorTextureId);
			return nullptr;
		}

		glRenderbufferStorage(renderbufferId, GL_DEPTH24_STENCIL8, width, height);

		GLuint framebufferId = 0;
		glCreateFramebuffers(1, &framebufferId);
		if (framebufferId == 0)
		{
			Error("Failed to create framebuffer");
			glDeleteRenderbuffers(1, &renderbufferId);
			glDeleteTextures(1, &colorTextureId);
			return nullptr;
		}

		glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);
		glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, colorTextureId, 0);
		glNamedFramebufferDrawBuffer(framebufferId, GL_COLOR_ATTACHMENT0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Error("Failed to create complete framebuffer");
			glDeleteFramebuffers(1, &framebufferId);
			glDeleteRenderbuffers(1, &renderbufferId);
			glDeleteTextures(1, &colorTextureId);
			return nullptr;
		}

		return std::unique_ptr<OpenGLFramebuffer>(new OpenGLFramebuffer(
			framebufferId,
			renderbufferId,
			colorTextureId,
			width, height
		));
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		if (m_FramebufferId != 0) glDeleteFramebuffers(1, &m_FramebufferId);
		if (m_RenderbufferId != 0) glDeleteRenderbuffers(1, &m_RenderbufferId);
		if (m_ColorTextureId != 0) glDeleteTextures(1, &m_ColorTextureId);
	}

	void OpenGLFramebuffer::Begin()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, m_Width, m_Height);
	}

	void OpenGLFramebuffer::End()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const GLuint framebufferId, const GLuint renderbufferId, const GLuint colorTextureId, const uint32_t width, const uint32_t height):
		m_FramebufferId(framebufferId),
		m_RenderbufferId(renderbufferId),
		m_ColorTextureId(colorTextureId),
		m_Width(width),
		m_Height(height)
	{
		Require(m_FramebufferId != 0, [] { return "Tried to create frame buffer with invalid Resource-Id."; });
		Require(m_RenderbufferId != 0, [] { return "Tried to create render buffer with invalid Resource-Id."; });
		Require(m_ColorTextureId != 0, [] { return "Tried to create frame buffer color texture with invalid Resource-Id."; });
	}
}