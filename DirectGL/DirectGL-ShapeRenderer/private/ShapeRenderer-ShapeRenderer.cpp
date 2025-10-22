module;

#include <Glad/gl.h>

#include <algorithm>
#include <bit>

module DirectGL.ShapeRenderer;

import Preconditions;

namespace DGL::ShapeRenderer
{
	[[nodiscard]] constexpr GLenum PrimitiveTypeToGlId(const PrimitiveType type)
	{
		switch (type)
		{
			case PrimitiveType::Points: return GL_POINTS;
			case PrimitiveType::Triangles: return GL_TRIANGLES;
			case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
			case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
			case PrimitiveType::Lines: return GL_LINES;
			case PrimitiveType::LineStrip: return GL_LINE_STRIP;
			case PrimitiveType::LineLoop: return GL_LINE_LOOP;
			default: System::Error("Unknown PrimitiveType");
		}
	}

	std::unique_ptr<ShapeRenderer> ShapeRenderer::Create(const size_t maxVertices, const size_t maxIndices)
	{
		GLuint buffers[2];
		glCreateBuffers(2, buffers);
		glNamedBufferStorage(buffers[0], maxIndices * sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT);
		glNamedBufferStorage(buffers[1], maxVertices * 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint vao = 0;
		glCreateVertexArrays(1, &vao);

		// Attach index buffer
		glVertexArrayElementBuffer(vao, buffers[0]);

		// Attach position buffer
		glVertexArrayVertexBuffer(vao, 0, buffers[1], 0, sizeof(GLfloat) * 3);
		glEnableVertexArrayAttrib(vao, 0);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vao, 0, 0);

		return std::unique_ptr<ShapeRenderer>(new ShapeRenderer(vao, buffers[1], buffers[0]));
	}

	ShapeRenderer::~ShapeRenderer()
	{
		if (m_VertexArrayId != 0) glDeleteVertexArrays(1, &m_VertexArrayId);
		if (m_PositionBufferId != 0) glDeleteBuffers(1, &m_PositionBufferId);
		if (m_IndexBufferId != 0) glDeleteBuffers(1, &m_IndexBufferId);
	}

	void ShapeRenderer::Render(const std::span<const float>& positions, const std::span<const uint32_t>& indices, const PrimitiveType type)
	{
		// Convert the primitive type to the corresponding OpenGL draw mode id
		const GLenum drawMode = PrimitiveTypeToGlId(type);

		// Upload the vertex positions to the GPU
		glNamedBufferSubData(m_PositionBufferId, 0, positions.size_bytes(), positions.data());

		// Bind the VAO to prepare for drawing
		glBindVertexArray(m_VertexArrayId);

		if (not indices.empty())
		{
			// Submit the index data to the GPU
			glNamedBufferSubData(m_IndexBufferId, 0, indices.size_bytes(), indices.data());

			// Render the shape using indexed drawing
			glDrawElements(drawMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
		} else
		{
			glDrawArrays(drawMode, 0, static_cast<GLsizei>(positions.size()) / 3);
		}
	}

	void ShapeRenderer::Render(const Vertices& vertices)
	{
		const auto rawPositions = std::bit_cast<const float*>(vertices.Positions.data());
		const size_t positionCount = vertices.Positions.size() * 3;

		const auto rawIndices = std::bit_cast<const uint32_t*>(vertices.Indices.data());
		const size_t indicesCount = vertices.Indices.size();

		Render(
			std::span{ rawPositions, positionCount },
			std::span{ rawIndices, indicesCount },
			vertices.Type
		);
	}

	ShapeRenderer::ShapeRenderer(const GLuint vertexArrayId, const GLuint positionBufferId, const GLuint indexBufferId):
		m_VertexArrayId(vertexArrayId),
		m_PositionBufferId(positionBufferId),
		m_IndexBufferId(indexBufferId)
	{
	}
}