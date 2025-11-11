module;

#include <glad/gl.h>

module DirectGL;

import :MainRenderTarget;

namespace DGL
{
	MainRenderTarget::MainRenderTarget(const Math::FloatBoundary& viewport):
		m_Viewport(viewport)
	{
	}

	void MainRenderTarget::SetViewport(const Math::FloatBoundary& viewport)
	{
		m_Viewport = viewport;
	}

	const Math::FloatBoundary& MainRenderTarget::GetViewport() const
	{
		return m_Viewport;
	}

	void MainRenderTarget::Activate()
	{
		glViewport(static_cast<GLsizei>(m_Viewport.Left), static_cast<GLsizei>(m_Viewport.Top), static_cast<GLsizei>(m_Viewport.Width), static_cast<GLsizei>(m_Viewport.Height));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}