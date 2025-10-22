module;

#include <Glad/gl.h>
#include <algorithm>

module DirectGL.TextureRenderer;

namespace DGL::TextureRenderer
{
	std::unique_ptr<TextureRenderer> TextureRenderer::Create(const size_t maxTexturesPerBatch)
	{
		// Generate the indices
		const std::unique_ptr<GLuint[]> indices(new GLuint[maxTexturesPerBatch * 6]);
		for (size_t i = 0; i < maxTexturesPerBatch; ++i)
		{
			const size_t baseVertex = i * 4;
			indices[i * 6 + 0] = static_cast<GLuint>(baseVertex + 0);
			indices[i * 6 + 1] = static_cast<GLuint>(baseVertex + 1);
			indices[i * 6 + 2] = static_cast<GLuint>(baseVertex + 2);
			indices[i * 6 + 3] = static_cast<GLuint>(baseVertex + 2);
			indices[i * 6 + 4] = static_cast<GLuint>(baseVertex + 3);
			indices[i * 6 + 5] = static_cast<GLuint>(baseVertex + 0);
		}
		
		// Generate the texture coordinates
		const std::unique_ptr<GLfloat[]> textureCoords(new GLfloat[maxTexturesPerBatch * 4 * 2]);
		for (size_t i = 0; i < maxTexturesPerBatch; ++i)
		{
			const size_t baseTexCoord = i * 4 * 2;
			textureCoords[baseTexCoord + 0] = 0.0f; textureCoords[baseTexCoord + 1] = 0.0f;
			textureCoords[baseTexCoord + 2] = 1.0f; textureCoords[baseTexCoord + 3] = 0.0f;
			textureCoords[baseTexCoord + 4] = 1.0f; textureCoords[baseTexCoord + 5] = 1.0f;
			textureCoords[baseTexCoord + 6] = 0.0f; textureCoords[baseTexCoord + 7] = 1.0f;
		}

		GLuint positionBufferId = 0;
		glCreateBuffers(1, &positionBufferId);
		glNamedBufferStorage(positionBufferId, maxTexturesPerBatch * 4 * 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_STORAGE_BIT);

		GLuint texCoordBufferId = 0;
		glCreateBuffers(1, &texCoordBufferId);
		glNamedBufferStorage(texCoordBufferId, maxTexturesPerBatch * 4 * 2 * sizeof(GLfloat), textureCoords.get(), GL_DYNAMIC_STORAGE_BIT);

		GLuint indexBufferId = 0;
		glCreateBuffers(1, &indexBufferId);
		glNamedBufferStorage(indexBufferId, maxTexturesPerBatch * 6 * sizeof(GLuint), indices.get(), GL_DYNAMIC_STORAGE_BIT);

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

		return std::unique_ptr<TextureRenderer>(new TextureRenderer(vertexArrayId, positionBufferId, texCoordBufferId, indexBufferId, maxTexturesPerBatch));
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
		// Check if we need to flush the current batch
		if (m_BatchedTextureCount >= m_MaxTexturesPerBatch)
		{
			Flush();
		}

		// Define the quad positions
		const GLfloat positions[] = {
			left, top, depth,
			left + width, top, depth,
			left + width, top + height, depth,
			left, top + height, depth,
		};

		// Copy the positions into their corresponding batch location
		const size_t batchOffset = m_BatchedTextureCount * std::size(positions);
		std::ranges::copy(positions, m_BatchedPositions.get() + batchOffset);

		// Increase the batched texture count
		++m_BatchedTextureCount;
	}

	void TextureRenderer::BeginDraw()
	{
		// Nothing to do for now.
	}

	void TextureRenderer::EndDraw()
	{
		Flush();
	}

	void TextureRenderer::Flush()
	{
		// Early out if there is nothing to render
		if (m_BatchedTextureCount == 0)
		{
			return;
		}

		printf("Flushing %zu batched textures\n", m_BatchedTextureCount);

		// Copy the data into the corresponding position buffer
		const size_t batchedPositionCount = m_BatchedTextureCount * 4 * 3;
		const size_t batchedPositionCountInBytes = batchedPositionCount * sizeof(GLfloat);
		glNamedBufferSubData(m_PositionBufferId, 0, batchedPositionCountInBytes, m_BatchedPositions.get());

		// Render the batched textures
		glBindVertexArray(m_VertexArrayId);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_BatchedTextureCount * 6), GL_UNSIGNED_INT, nullptr);

		// Reset the batched texture count
		m_BatchedTextureCount = 0;
	}

	TextureRenderer::TextureRenderer(const GLuint vertexArrayId, const GLuint positionBufferId, const GLuint texCoordBufferId, const GLuint indexBufferId, const size_t maxTexturesPerBatch):
		m_VertexArrayId(vertexArrayId),
		m_PositionBufferId(positionBufferId),
		m_TexCoordBufferId(texCoordBufferId),
		m_IndexBufferId(indexBufferId),
		m_BatchedPositions(new float[maxTexturesPerBatch * 4 * 3]),
		m_BatchedTextureCount(0),
		m_MaxTexturesPerBatch(maxTexturesPerBatch)
	{
	}
}