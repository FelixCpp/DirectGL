// Project Name : DirectGL
// File Name    : DGL-OpenGLTextureSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <memory>

#include <glad/gl.h>

export module DGL:OpenGLTextureSampler;

import :TextureSampler;

namespace DGL
{
	class OpenGLTextureSampler : public TextureSampler
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLTextureSampler> Create(TextureWrapMode wrapMode, TextureFilterMode filterMode);

		~OpenGLTextureSampler() override;

		void SetWrapMode(TextureWrapMode mode) override;
		[[nodiscard]] TextureWrapMode GetWrapMode() const override;

		void SetFilterMode(TextureFilterMode mode) override;
		[[nodiscard]] TextureFilterMode GetFilterMode() const override;

		void Bind(uint32_t slot) const;

	private:

		explicit OpenGLTextureSampler(GLuint id, TextureWrapMode wrapMode, TextureFilterMode filterMode);

	private:

		GLuint m_SamplerId;

		TextureWrapMode m_WrapMode;
		TextureFilterMode m_FilterMode;

	};
}