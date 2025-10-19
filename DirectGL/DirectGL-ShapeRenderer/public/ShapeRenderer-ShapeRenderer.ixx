// Project Name : DirectGL-ShapeRenderer
// File Name    : ShapeRenderer-ShapeRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

module;

#include <Glad/gl.h>

#include <memory>
#include <span>

export module DirectGL.ShapeRenderer:ShapeRenderer;

import :Vertices;
import :PrimitiveType;

export namespace DGL::ShapeRenderer
{
	class ShapeRenderer
	{
	public:

		/// @brief Create a new ShapeRenderer instance.
		/// @param maxVertices The number of vertices that can be rendered in a single draw call (batch).
		/// @param maxIndices The number of indices that can be rendered in a single draw call (batch).
		/// @return A unique pointer to the created ShapeRenderer instance.
		static std::unique_ptr<ShapeRenderer> Create(size_t maxVertices, size_t maxIndices);

		~ShapeRenderer();

		/// @brief Submit a list of positions to be rendered as triangles
		/// @param positions A contiguous array of positions (x, y, z) to be submitted to the GPU
		/// @param indices A contiguous array of indices into the position array, defining which positions to render
		/// @param type The primitive type to render
		void Render(const std::span<const float>& positions, const std::span<const uint32_t>& indices, PrimitiveType type);

		/// @brief Submit a list of vertices to be rendered as triangles
		/// @param vertices The vertices to be submitted to the GPU
		void Render(const Vertices& vertices);

	private:

		explicit ShapeRenderer(
			GLuint vertexArrayId,
			GLuint positionBufferId,
			GLuint indexBufferId
		);

		GLuint m_VertexArrayId;
		GLuint m_PositionBufferId;
		GLuint m_IndexBufferId;

	};
}