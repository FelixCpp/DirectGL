// Project Name : DirectGL-Renderer
// File Name    : Renderer-VertexRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL.Renderer:VertexRenderer;

import :Vertices;

export namespace DGL::Renderer
{
	class VertexRenderer
	{
	public:

		static std::unique_ptr<VertexRenderer> Create(size_t maxVertices);

		~VertexRenderer();

		void Render(const Vertices& vertices);

	private:

		explicit VertexRenderer(
			GLuint vertexArrayId,
			GLuint vertexBufferId,
			GLuint indexBufferId
		);

		GLuint m_VertexArrayId;
		GLuint m_VertexBufferId;
		GLuint m_IndexBufferId;

	};
}