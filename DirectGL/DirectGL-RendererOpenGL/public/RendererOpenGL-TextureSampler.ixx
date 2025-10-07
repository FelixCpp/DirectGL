// Project Name : DirectGL-RendererOpenGL
// File Name    : RendererOpenGL-TextureSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL.RendererOpenGL:TextureSampler;

import DirectGL.Renderer;

export namespace DGL::RendererOpenGL
{
	class OpenGLTextureSampler : public Renderer::TextureSampler
	{
	public:

		static std::unique_ptr<OpenGLTextureSampler> Create(
			Renderer::TextureWrapMode wrapMode,
			Renderer::TextureFilterMode filterMode
		);

		~OpenGLTextureSampler() override;

		void SetWrapMode(Renderer::TextureWrapMode mode) override;
		Renderer::TextureWrapMode GetWrapMode() const override;

		void SetFilterMode(Renderer::TextureFilterMode mode) override;
		Renderer::TextureFilterMode GetFilterMode() const override;

		GLuint GetRendererId() const;

	private:

		explicit OpenGLTextureSampler(
			GLuint id,
			Renderer::TextureWrapMode wrapMode,
			Renderer::TextureFilterMode filterMode
		);

		GLuint m_RendererId;
		Renderer::TextureWrapMode m_WrapMode;
		Renderer::TextureFilterMode m_FilterMode;

	};
}