
module;

#include <glad/gl.h>

module DirectGL;

import :MainGraphicsLayer;

namespace DGL
{
	std::unique_ptr<MainGraphicsLayer> MainGraphicsLayer::Create(
		const Math::Uint2 viewportSize,
		Renderer::Renderer& renderer,
		Renderer::ShapeFactory& shapeFactory
	) {
		return std::unique_ptr<MainGraphicsLayer>(new MainGraphicsLayer(viewportSize, renderer, shapeFactory));
	}

	void MainGraphicsLayer::Resize(const uint32_t width, const uint32_t height)
	{
		m_Viewport = Math::FloatBoundary::FromLTWH(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
		m_ProjectionMatrix = Math::Matrix4x4::Orthographic(m_Viewport, -1.0f, 1.0f);
		glViewport(0, 0, width, height);
	}

	void MainGraphicsLayer::BeginDraw()
	{
		// Call the base class implementation before doing anything on our own
		BaseGraphicsLayer::BeginDraw();

		// Prevent nested BeginDraw calls
		if (m_IsDrawing)
		{
			Warning("MainGraphicsLayer::BeginDraw() called while already drawing. Nested BeginDraw calls are not allowed.");
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
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, m_Viewport.Width, m_Viewport.Height);
	}

	void MainGraphicsLayer::EndDraw()
	{
		// Prevent EndDraw calls without a preceding BeginDraw call
		if (not m_IsDrawing)
		{
			Warning("MainGraphicsLayer::EndDraw() called without a preceding BeginDraw() call.");
			return;
		}

		m_IsDrawing = false;

		// Restore the previously bound framebuffer & viewport
		glBindFramebuffer(GL_FRAMEBUFFER, m_CachedFramebufferId);
		glViewport(m_CachedViewport.Left, m_CachedViewport.Top, m_CachedViewport.Width, m_CachedViewport.Height);
	}

	MainGraphicsLayer::MainGraphicsLayer(const Math::Uint2 viewportSize, Renderer::Renderer& renderer, Renderer::ShapeFactory& shapeFactory) :
		BaseGraphicsLayer(viewportSize, renderer, shapeFactory),
		m_CachedFramebufferId(0),
		m_CachedViewport(Math::UintBoundary::Zero),
		m_IsDrawing(false)
	{
	}
}
