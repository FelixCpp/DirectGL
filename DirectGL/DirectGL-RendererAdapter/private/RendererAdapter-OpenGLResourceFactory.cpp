module DirectGL.RendererAdapter;

import :OpenGLResourceFactory;

import DirectGL.RendererOpenGL;

namespace DGL::RendererAdapter
{
	std::unique_ptr<Renderer::TextureSampler> OpenGLResourceFactory::CreateTextureSampler(const Renderer::TextureWrapMode wrapMode, const Renderer::TextureFilterMode filterMode)
	{
		return RendererOpenGL::OpenGLTextureSampler::Create(wrapMode, filterMode);
	}
}