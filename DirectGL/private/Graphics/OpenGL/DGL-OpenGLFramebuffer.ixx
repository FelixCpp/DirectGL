// Project Name : DirectGL
// File Name    : DGL-OpenGLFramebuffer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

module;

#include <glad/gl.h>

#include <memory>

export module DGL:OpenGLFramebuffer;

import :Framebuffer;

export namespace DGL
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLFramebuffer> Create(uint32_t width, uint32_t height);

		~OpenGLFramebuffer() override;

		void Begin() override;
		void End() override;

	private:

		explicit OpenGLFramebuffer(
			GLuint framebufferId,
			GLuint renderbufferId,
			GLuint colorTextureId,
			uint32_t width,
			uint32_t height
		);

		GLuint m_FramebufferId;
		GLuint m_RenderbufferId;

		GLuint m_ColorTextureId;

		uint32_t m_Width;
		uint32_t m_Height;

	};
}