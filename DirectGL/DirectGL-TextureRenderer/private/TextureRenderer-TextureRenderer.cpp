module;

#include <Glad/gl.h>

module DirectGL.TextureRenderer;

namespace DGL::TextureRenderer
{
	std::unique_ptr<TextureRenderer> TextureRenderer::Create(const size_t maxVertices)
	{
		GLuint positionBufferId = 0;
		glCreateBuffers(1, &positionBufferId);
		glNamedBufferStorage(positionBufferId, maxVertices * 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint texCoordBufferId = 0;
		glCreateBuffers(1, &texCoordBufferId);
		glNamedBufferStorage(texCoordBufferId, maxVertices * 2 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint indexBufferId = 0;
		glCreateBuffers(1, &indexBufferId);
		glNamedBufferStorage(indexBufferId, maxVertices * sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint vertexArrayId = 0;
		glCreateVertexArrays(1, &vertexArrayId);
		glVertexArrayElementBuffer(vertexArrayId, indexBufferId);

		glVertexArrayVertexBuffer(vertexArrayId, 0, positionBufferId, 0, 3 * sizeof(GLfloat));
		glEnableVertexArrayAttrib(vertexArrayId, 0);
		glVertexArrayAttribFormat(vertexArrayId, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vertexArrayId, 0, 0);

		glVertexArrayVertexBuffer(vertexArrayId, 1, texCoordBufferId, 0, 2 * sizeof(GLfloat));
		glEnableVertexArrayAttrib(vertexArrayId, 1);
		glVertexArrayAttribFormat(vertexArrayId, 1, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vertexArrayId, 1, 1);

		return std::unique_ptr<TextureRenderer>(new TextureRenderer(vertexArrayId, positionBufferId, texCoordBufferId, indexBufferId));
	}

	TextureRenderer::~TextureRenderer()
	{
		if (m_PositionBufferId != 0) glDeleteBuffers(1, &m_PositionBufferId);
		if (m_TexCoordBufferId != 0) glDeleteBuffers(1, &m_TexCoordBufferId);
		if (m_IndexBufferId != 0) glDeleteBuffers(1, &m_IndexBufferId);
		if (m_VertexArrayId != 0) glDeleteVertexArrays(1, &m_VertexArrayId);
	}

	void TextureRenderer::Render(const float left, const float top, const float width, const float height, const float depth)
	{
		const GLfloat positions[] = {
			left, top, depth,
			left + width, top, depth,
			left + width, top + height, depth,
			left, top + height, depth,
		};

		constexpr GLfloat texCoords[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
		};

		constexpr GLuint indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		glNamedBufferSubData(m_PositionBufferId, 0, sizeof(positions), positions);
		glNamedBufferSubData(m_TexCoordBufferId, 0, sizeof(texCoords), texCoords);
		glNamedBufferSubData(m_IndexBufferId, 0, sizeof(indices), indices);
		glBindVertexArray(m_VertexArrayId);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	TextureRenderer::TextureRenderer(const GLuint vertexArrayId, const GLuint positionBufferId, const GLuint texCoordBufferId, const GLuint indexBufferId):
		m_VertexArrayId(vertexArrayId),
		m_PositionBufferId(positionBufferId),
		m_TexCoordBufferId(texCoordBufferId),
		m_IndexBufferId(indexBufferId)
	{
	}
}