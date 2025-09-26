module;

#include <glad/gl.h>
#include <cstdint>

module DGL;

namespace DGL
{
	inline static constexpr uint64_t MAX_VERTICES = 10'000;

	OpenGLRenderer::OpenGLRenderer() :
		m_Vao(0),
		m_PositionVbo(0),
		m_TextureVbo(0),
		m_Ebo(0)
	{
		// Generate Position VBO
		glCreateBuffers(1, &m_PositionVbo);
		glNamedBufferStorage(m_PositionVbo, MAX_VERTICES * sizeof(GLfloat) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate Texture VBO
		glCreateBuffers(1, &m_TextureVbo);
		glNamedBufferStorage(m_TextureVbo, MAX_VERTICES * sizeof(GLfloat) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate EBO
		glCreateBuffers(1, &m_Ebo);
		glNamedBufferStorage(m_Ebo, MAX_VERTICES * sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate VAO
		glCreateVertexArrays(1, &m_Vao);

		// Link VBO, EBO to VAO
		glVertexArrayElementBuffer(m_Vao, m_Ebo);

		// Position attribute
		glVertexArrayVertexBuffer(m_Vao, 0, m_PositionVbo, 0, sizeof(GLfloat) * 2);
		glVertexArrayAttribBinding(m_Vao, 0, 0);
		glVertexArrayAttribFormat(m_Vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(m_Vao, 0);

		// Texture-Coord attribute
		glVertexArrayVertexBuffer(m_Vao, 1, m_TextureVbo, 0, sizeof(GLfloat) * 2);
		glVertexArrayAttribBinding(m_Vao, 1, 1);
		glVertexArrayAttribFormat(m_Vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(m_Vao, 1);
	}

	OpenGLRenderer::~OpenGLRenderer()
	{
		if (m_Ebo != 0)
		{
			glDeleteBuffers(1, &m_Ebo);
		}

		if (m_PositionVbo != 0)
		{
			glDeleteBuffers(1, &m_PositionVbo);
		}

		if (m_TextureVbo != 0)
		{
			glDeleteBuffers(1, &m_TextureVbo);
		}

		if (m_Vao != 0)
		{
			glDeleteVertexArrays(1, &m_Vao);
		}
	}

	void OpenGLRenderer::Submit(const Geometry& geometry)
	{
		glNamedBufferSubData(m_PositionVbo, 0, geometry.Positions.size() * sizeof(GLfloat) * 2, geometry.Positions.data());
		glNamedBufferSubData(m_TextureVbo, 0, geometry.TexCoords.size() * sizeof(GLfloat) * 2, geometry.TexCoords.data());

		const size_t indexCount = geometry.Indices.size();
		glNamedBufferSubData(m_Ebo, 0, indexCount * sizeof(GLuint), geometry.Indices.data());

		glBindVertexArray(m_Vao);
#if 0
		glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(geometry.Positions.size()));
		//glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(geometry.Positions.size()));
#else
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
#endif
	}
}