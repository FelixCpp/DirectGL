module;

#include <glad/gl.h>

module DirectGL.Renderer;

import Math;

namespace DGL::Renderer
{
	std::unique_ptr<VertexRenderer> VertexRenderer::Create(const size_t maxVertices)
	{
		// Create position vertex buffer object (VBO)
		GLuint vertexBuffer = 0;
		glCreateBuffers(1, &vertexBuffer);
		glNamedBufferStorage(vertexBuffer, maxVertices * sizeof(Math::Float2), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Create index buffer object (EBO/IBO)
		GLuint elementBuffer = 0;
		glCreateBuffers(1, &elementBuffer);
		glNamedBufferStorage(elementBuffer, maxVertices * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Create vertex array object (VAO)
		GLuint vertexArrayId = 0;
		glCreateVertexArrays(1, &vertexArrayId);

		// Attach the element buffer and vertex buffer to the VAO
		glVertexArrayElementBuffer(vertexArrayId, elementBuffer);
		glVertexArrayVertexBuffer(vertexArrayId, 0, vertexBuffer, 0, sizeof(Math::Float2));

		// Define the vertex attribute layout (position)
		glEnableVertexArrayAttrib(vertexArrayId, 0);
		glVertexArrayAttribFormat(vertexArrayId, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vertexArrayId, 0, 0);

		return std::unique_ptr<VertexRenderer>(new VertexRenderer(vertexArrayId, vertexBuffer, elementBuffer));
	}

	VertexRenderer::~VertexRenderer()
	{
		if (m_VertexArrayId != 0) glDeleteVertexArrays(1, &m_VertexArrayId);
		if (m_VertexBufferId != 0) glDeleteBuffers(1, &m_VertexBufferId);
		if (m_IndexBufferId != 0) glDeleteBuffers(1, &m_IndexBufferId);
	}

	void VertexRenderer::Render(const Vertices& vertices)
	{
		// TODO(Felix): What's up with buffer overflows?

		// Submit the vertex and index data to the GPU
		glNamedBufferSubData(m_VertexBufferId, 0, vertices.Positions.size() * sizeof(Math::Float2), vertices.Positions.data());
		glNamedBufferSubData(m_IndexBufferId, 0, vertices.Indices.size() * sizeof(uint32_t), vertices.Indices.data());

		// Bind the VAO and draw the elements
		glBindVertexArray(m_VertexArrayId);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertices.Indices.size()), GL_UNSIGNED_INT, nullptr);
	}

	VertexRenderer::VertexRenderer(const GLuint vertexArrayId, const GLuint vertexBufferId, const GLuint indexBufferId):
		m_VertexArrayId(vertexArrayId),
		m_VertexBufferId(vertexBufferId),
		m_IndexBufferId(indexBufferId)
	{
	}
}