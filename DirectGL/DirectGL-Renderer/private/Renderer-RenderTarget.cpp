module;

#include <glad/gl.h>

#include <optional>
#include <type_traits>

module DirectGL.Renderer;

import DirectGL.Logging;

namespace DGL::Renderer
{
	[[nodiscard]] RenderTargetRestoreInformation QueryCurrentRestoreInfo()
	{
		GLint currentFramebufferId = 0;
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFramebufferId);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		return RenderTargetRestoreInformation{
			.CachedFramebufferId = currentFramebufferId,
			.CachedViewport = Math::UintBoundary::FromLTWH(viewport[0], viewport[1], viewport[2], viewport[3])
		};
	}

	void RestoreRenderTarget(const RenderTargetRestoreInformation& info)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(info.CachedFramebufferId));
		const auto [x, y, width, height] = info.CachedViewport;

		glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
	}
}

namespace DGL::Renderer
{
	std::unique_ptr<MainRenderTarget> MainRenderTarget::Create(const Math::UintBoundary viewport)
	{
		return std::unique_ptr<MainRenderTarget>(new MainRenderTarget(viewport));
	}

	void MainRenderTarget::SetViewport(const Math::UintBoundary viewport)
	{
		if (m_RestoreInfo != std::nullopt)
		{
			Logging::Debug("MainRenderTarget::SetViewport() called while drawing. The new viewport will take effect after EndDraw().");
		}

		m_Viewport = viewport;
	}

	const Math::UintBoundary& MainRenderTarget::GetViewport() const
	{
		return m_Viewport;
	}

	void MainRenderTarget::BeginDraw()
	{
		if (m_RestoreInfo != std::nullopt)
		{
			Logging::Warning("MainRenderTarget::BeginDraw() called while already drawing. Nested BeginDraw calls are not allowed.");
			return;
		}

		m_RestoreInfo = QueryCurrentRestoreInfo();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(m_Viewport.Left, m_Viewport.Top, m_Viewport.Width, m_Viewport.Height);
	}

	void MainRenderTarget::EndDraw()
	{
		if (const auto restoreInfo = m_RestoreInfo)
		{
			RestoreRenderTarget(*restoreInfo);
			m_RestoreInfo.reset();
		} else
		{
			Logging::Warning("MainRenderTarget::EndDraw() called, but no restore information is available. This indicates a logic error.");
		}
	}

	MainRenderTarget::MainRenderTarget(const Math::UintBoundary viewport):
		m_Viewport(viewport)
	{
	}
}

namespace DGL::Renderer
{
	std::unique_ptr<OffscreenRenderTarget> OffscreenRenderTarget::Create(const Math::Uint2 viewportSize)
	{
		if (viewportSize.X == 0 or viewportSize.Y == 0)
		{
			Logging::Error("OffscreenRenderTarget::Create() called with zero width or height.");
			return nullptr;
		}

		auto renderTexture = Texture::Texture::Create(viewportSize, nullptr);
		if (renderTexture == nullptr)
		{
			Logging::Error("Failed to create render texture for OffscreenRenderTarget.");
			return nullptr;
		}

		GLuint renderbufferId = 0;
		glCreateRenderbuffers(1, &renderbufferId);
		glNamedRenderbufferStorage(renderbufferId, GL_DEPTH24_STENCIL8, viewportSize.X, viewportSize.Y);

		GLuint framebufferId = 0;
		glCreateFramebuffers(1, &framebufferId);
		glNamedFramebufferTexture(framebufferId, GL_COLOR_ATTACHMENT0, renderTexture->GetRendererId(), 0);
		glNamedFramebufferRenderbuffer(framebufferId, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferId);

		if (glCheckNamedFramebufferStatus(framebufferId, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			glDeleteRenderbuffers(1, &renderbufferId);
			glDeleteFramebuffers(1, &framebufferId);
			Logging::Error("Failed to create complete framebuffer for OffscreenRenderTarget.");
			return nullptr;
		}

		return std::unique_ptr<OffscreenRenderTarget>(new OffscreenRenderTarget(viewportSize, framebufferId, renderbufferId, std::move(renderTexture)));
	}

	OffscreenRenderTarget::~OffscreenRenderTarget()
	{
		if (m_RestoreInfo != std::nullopt) RestoreRenderTarget(*m_RestoreInfo);
		if (m_FramebufferId != 0) glDeleteFramebuffers(1, &m_FramebufferId);
		if (m_RenderbufferId != 0) glDeleteRenderbuffers(1, &m_RenderbufferId);
	}

	void OffscreenRenderTarget::BeginDraw()
	{
		if (m_RestoreInfo != std::nullopt)
		{
			Logging::Warning("OffscreenRenderTarget::BeginDraw() called while already drawing. Nested BeginDraw calls are not allowed.");
			return;
		}

		m_RestoreInfo = QueryCurrentRestoreInfo();

		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, m_ViewportSize.X, m_ViewportSize.Y);
	}

	void OffscreenRenderTarget::EndDraw()
	{
		if (const auto restoreInfo = m_RestoreInfo)
		{
			RestoreRenderTarget(*restoreInfo);
		} else
		{
			Logging::Warning("OffscreenRenderTarget::EndDraw() called, but no restore information is available. This indicates a logic error.");
		}
	}

	const Texture::Texture& OffscreenRenderTarget::GetRenderTexture() const
	{
		return *m_RenderTexture;
	}

	OffscreenRenderTarget::OffscreenRenderTarget(const Math::Uint2 viewportSize, const GLuint framebufferId, const GLuint renderbufferId, std::unique_ptr<Texture::Texture> renderTexture):
		m_FramebufferId(framebufferId),
		m_RenderbufferId(renderbufferId),
		m_RenderTexture(std::move(renderTexture)),
		m_ViewportSize(viewportSize)
	{
	}

}