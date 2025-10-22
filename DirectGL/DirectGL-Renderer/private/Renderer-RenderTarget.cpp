module;

#include <glad/gl.h>

#include <optional>
#include <memory>
#include <type_traits>
#include <format>

module DirectGL.Renderer;

import DirectGL.Logging;

namespace DGL::Renderer
{
	std::unique_ptr<MainRenderTarget> MainRenderTarget::Create(const Math::UintBoundary viewport)
	{
		return std::unique_ptr<MainRenderTarget>(new MainRenderTarget(viewport));
	}

	void MainRenderTarget::SetViewport(const Math::UintBoundary viewport)
	{
		m_Viewport = viewport;
	}

	const Math::UintBoundary& MainRenderTarget::GetViewport() const
	{
		return m_Viewport;
	}

	void MainRenderTarget::Activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(m_Viewport.Left, m_Viewport.Top, m_Viewport.Width, m_Viewport.Height);
	//	Logging::Info(std::format("Activated MainRenderTarget with viewport size {}x{}", m_Viewport.Width, m_Viewport.Height));
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
		if (m_FramebufferId != 0) glDeleteFramebuffers(1, &m_FramebufferId);
		if (m_RenderbufferId != 0) glDeleteRenderbuffers(1, &m_RenderbufferId);
	}

	void OffscreenRenderTarget::Activate()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
		glViewport(0, 0, m_ViewportSize.X, m_ViewportSize.Y);
	//	Logging::Info(std::format("Activated OffscreenRenderTarget with viewport size {}x{}", m_ViewportSize.X, m_ViewportSize.Y));
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