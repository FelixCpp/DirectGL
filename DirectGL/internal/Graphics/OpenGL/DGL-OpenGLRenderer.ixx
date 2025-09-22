// Project Name : DirectGL
// File Name    : DGL-OpenGLRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

export module DGL:OpenGLRenderer;

import :Renderer;

namespace DGL
{
	class OpenGLRenderer : public Renderer
	{
	public:

		OpenGLRenderer();
		~OpenGLRenderer() override;

		OpenGLRenderer(const OpenGLRenderer&) = delete;
		OpenGLRenderer& operator=(const OpenGLRenderer&) = delete;
		OpenGLRenderer(OpenGLRenderer&& other) noexcept;
		OpenGLRenderer& operator=(OpenGLRenderer&& other) noexcept;

		void Submit(const Geometry& geometry) override;

	private:

		GLuint m_Vao;
		GLuint m_PositionVbo;
		GLuint m_TextureVbo;
		GLuint m_Ebo;

	};
}