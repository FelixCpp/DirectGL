// Project Name : DirectGL
// File Name    : DGL-OpenGLRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

#include <memory>

export module DGL:OpenGLRenderer;

import :Renderer;
import :ShaderProgram;

namespace DGL
{
	class OpenGLRenderer : public Renderer
	{
	public:

		static std::unique_ptr<OpenGLRenderer> Create(uint32_t maxVertices);
		~OpenGLRenderer() override;

		void Submit(const Geometry& geometry) override;

	private:

		explicit OpenGLRenderer(
			uint32_t maxVertices,
			GLuint vao,
			GLuint positionVbo,
			GLuint textureVbo,
			GLuint ebo
		);

	private:

		GLuint m_Vao;
		GLuint m_PositionVbo;
		GLuint m_TextureVbo;
		GLuint m_Ebo;

		uint32_t m_MaxVertices;

	};
}