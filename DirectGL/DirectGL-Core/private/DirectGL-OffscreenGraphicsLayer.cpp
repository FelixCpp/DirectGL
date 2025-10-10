module;

#include <glad/gl.h>

module DirectGL;

import :OffscreenGraphicsLayer;

namespace DGL
{
	std::unique_ptr<OffscreenGraphicsLayer> OffscreenGraphicsLayer::Create(Math::Uint2 viewportSize, Renderer::Renderer& renderer, Renderer::ShapeFactory& shapeFactory)
	{
		const auto [width, height] = viewportSize;

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

		return std::unique_ptr<OffscreenGraphicsLayer>(new OffscreenGraphicsLayer(viewportSize, renderer, shapeFactory, framebufferId, renderbufferId, textureId));
	}

	OffscreenGraphicsLayer::~OffscreenGraphicsLayer()
	{
		if (m_FramebufferId != 0) glDeleteFramebuffers(1, &m_FramebufferId);
		if (m_RenderbufferId != 0) glDeleteRenderbuffers(1, &m_RenderbufferId);
		if (m_RenderTextureId != 0) glDeleteTextures(1, &m_RenderTextureId);
	}

	void OffscreenGraphicsLayer::BeginDraw()
	{
		// Call the base class implementation before doing anything on our own
		BaseGraphicsLayer::BeginDraw();

		// Prevent nested BeginDraw calls
		if (m_IsDrawing)
		{
			Warning("OffscreenGraphicsLayer::BeginDraw() called while already drawing. Nested BeginDraw calls are not allowed.");
			return;
		}

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
		{
			Warning("OffscreenGraphicsLayer::EndDraw() called without a preceding BeginDraw() call.");
			return;
		}

		m_IsDrawing = false;

		// Restore the previously bound framebuffer & viewport
		glBindFramebuffer(GL_FRAMEBUFFER, m_CachedFramebufferId);
		glViewport(m_CachedViewport.Left, m_CachedViewport.Top, m_CachedViewport.Width, m_CachedViewport.Height);
	}

	OffscreenGraphicsLayer::OffscreenGraphicsLayer(
		const Math::Uint2 viewportSize,
		Renderer::Renderer& renderer,
		Renderer::ShapeFactory& shapeFactory,
		const GLuint framebufferId,
		const GLuint renderbufferId,
		const GLuint renderTextureId
	) : BaseGraphicsLayer(viewportSize, renderer, shapeFactory),
		m_FramebufferId(framebufferId),
		m_RenderbufferId(renderbufferId),
		m_RenderTextureId(renderTextureId),
		m_CachedFramebufferId(0),
		m_IsDrawing(false)
	{
	}
}
