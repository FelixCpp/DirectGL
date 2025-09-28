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

		[[nodiscard]] static std::unique_ptr<OpenGLTextureSampler> Create();

		~OpenGLTextureSampler() override;

		void Bind(uint32_t slot) const;

	private:

		explicit OpenGLTextureSampler(GLuint id);

	private:

		GLuint m_SamplerId;

	};
}