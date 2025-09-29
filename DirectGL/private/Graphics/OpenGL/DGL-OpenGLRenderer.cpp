module;

#include <glad/gl.h>
#include <cstdint>
#include <memory>

module DGL;

import :OpenGLShaderProgram;

namespace DGL
{
	std::unique_ptr<OpenGLRenderer> OpenGLRenderer::Create(const uint32_t maxVertices)
	{
		// Generate Position VBO
		GLuint positionVbo = 0;
		glCreateBuffers(1, &positionVbo);
		if (positionVbo == 0)
		{
			Error("Failed to create position VBO");
			return nullptr;
		}

		glNamedBufferStorage(positionVbo, maxVertices * sizeof(GLfloat) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate Texture VBO
		GLuint textureVbo = 0;
		glCreateBuffers(1, &textureVbo);
		if (textureVbo == 0)
		{
			Error("Failed to create texture VBO");
			glDeleteBuffers(1, &positionVbo);
			return nullptr;
		}

		glNamedBufferStorage(textureVbo, maxVertices * sizeof(GLfloat) * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate EBO
		GLuint ebo = 0;
		glCreateBuffers(1, &ebo);
		if (ebo == 0)
		{
			Error("Failed to create EBO");
			glDeleteBuffers(1, &positionVbo);
			glDeleteBuffers(1, &textureVbo);
			return nullptr;
		}

		glNamedBufferStorage(ebo, maxVertices * sizeof(GLuint), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Generate VAO
		GLuint vao = 0;
		glCreateVertexArrays(1, &vao);
		if (vao == 0)
		{
			Error("Failed to create VAO");
			glDeleteBuffers(1, &positionVbo);
			glDeleteBuffers(1, &textureVbo);
			glDeleteBuffers(1, &ebo);
			return nullptr;
		}

		// Link VBO, EBO to VAO
		glVertexArrayElementBuffer(vao, ebo);

		// Position attribute
		glVertexArrayVertexBuffer(vao, 0, positionVbo, 0, sizeof(GLfloat) * 2);
		glVertexArrayAttribBinding(vao, 0, 0);
		glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(vao, 0);

		// Texture-Coord attribute
		glVertexArrayVertexBuffer(vao, 1, textureVbo, 0, sizeof(GLfloat) * 2);
		glVertexArrayAttribBinding(vao, 1, 1);
		glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(vao, 1);

		return std::unique_ptr<OpenGLRenderer>(new OpenGLRenderer(maxVertices, vao, positionVbo, textureVbo, ebo));
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
		glNamedBufferSubData(m_Ebo, 0, geometry.Indices.size() * sizeof(GLuint), geometry.Indices.data());

		glBindVertexArray(m_Vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(geometry.Indices.size()), GL_UNSIGNED_INT, nullptr);
	}

	OpenGLRenderer::OpenGLRenderer(const uint32_t maxVertices, const GLuint vao, const GLuint positionVbo, const GLuint textureVbo, const GLuint ebo):
		m_MaxVertices(maxVertices),
		m_Vao(vao),
		m_PositionVbo(positionVbo),
		m_TextureVbo(textureVbo),
		m_Ebo(ebo)
	{
	}
}