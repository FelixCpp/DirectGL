// Project Name : DirectGL-Renderer
// File Name    : Renderer-VertexRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL.Renderer:VertexRenderer;

import :BlendMode;
import :Vertices;
import :Renderer;

export namespace DGL::Renderer
{
	class VertexRenderer : public Renderer
	{
	public:

		static std::unique_ptr<VertexRenderer> Create(size_t maxVertices);

		~VertexRenderer() override;
 
		void Render(const Vertices& vertices, const BlendMode& blendMode) override;

	private:

		explicit VertexRenderer(
			GLuint vertexArrayId,
			GLuint vertexBufferId,
			GLuint indexBufferId
		);

		void Activate(const BlendMode& blendMode);

		GLuint m_VertexArrayId;
		GLuint m_VertexBufferId;
		GLuint m_IndexBufferId;

	};
}