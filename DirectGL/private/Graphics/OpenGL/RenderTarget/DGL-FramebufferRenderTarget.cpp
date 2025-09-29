module DGL;

namespace DGL
{
	FramebufferRenderTarget::FramebufferRenderTarget(std::unique_ptr<Framebuffer> framebuffer, Renderer* renderer):
		m_Framebuffer(std::move(framebuffer)),
		m_Renderer(renderer)
	{
	}

	void FramebufferRenderTarget::BeginDraw()
	{
		m_Framebuffer->Begin();
	}

	void FramebufferRenderTarget::EndDraw()
	{
		m_Framebuffer->End();
	}
}
