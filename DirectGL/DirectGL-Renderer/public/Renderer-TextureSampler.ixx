// Project Name : DirectGL-Renderer
// File Name    : Renderer-TextureSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL.Renderer:TextureSampler;

import :TextureFilterMode;
import :TextureWrapMode;

export namespace DGL::Renderer
{
	class TextureSampler
	{
	public:

		static std::unique_ptr<TextureSampler> Create(
			TextureFilterMode filterMode = TextureFilterMode::Linear,
			TextureWrapMode wrapMode = TextureWrapMode::ClampToEdge
		);

		~TextureSampler();

		void SetFilterMode(TextureFilterMode mode);
		TextureFilterMode GetFilterMode() const;

		void SetWrapMode(TextureWrapMode mode);
		TextureWrapMode GetWrapMode() const;

		GLuint GetRendererId() const;

	private:

		explicit TextureSampler(
			TextureFilterMode filterMode,
			TextureWrapMode wrapMode,
			GLuint samplerId
		);

		GLuint m_SamplerId;
		TextureFilterMode m_FilterMode;
		TextureWrapMode m_WrapMode;

	};
}