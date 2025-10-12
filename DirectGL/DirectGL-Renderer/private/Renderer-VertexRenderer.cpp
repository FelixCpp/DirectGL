module;

#include <glad/gl.h>

module DirectGL.Renderer;

import DirectGL.Math;

namespace DGL::Renderer
{
	constexpr GLenum BlendFactorToGlId(const BlendMode::Factor factor)
	{
		switch (factor)
		{
			case BlendMode::Factor::Zero: return GL_ZERO;
			case BlendMode::Factor::One: return GL_ONE;
			case BlendMode::Factor::SrcColor: return GL_SRC_COLOR;
			case BlendMode::Factor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
			case BlendMode::Factor::DstColor: return GL_DST_COLOR;
			case BlendMode::Factor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
			case BlendMode::Factor::SrcAlpha: return GL_SRC_ALPHA;
			case BlendMode::Factor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
			case BlendMode::Factor::DstAlpha: return GL_DST_ALPHA;
			case BlendMode::Factor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
			case BlendMode::Factor::ConstantColor: return GL_CONSTANT_COLOR;
			case BlendMode::Factor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
			case BlendMode::Factor::ConstantAlpha: return GL_CONSTANT_ALPHA;
			case BlendMode::Factor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
			case BlendMode::Factor::SrcAlphaSaturate: return GL_SRC_ALPHA_SATURATE;
			default: return GL_INVALID_ENUM;
		}
	}

	constexpr GLenum BlendEquationToGlId(const BlendMode::Equation equation)
	{
		switch (equation)
		{
			case BlendMode::Equation::Add: return GL_FUNC_ADD;
			case BlendMode::Equation::Subtract: return GL_FUNC_SUBTRACT;
			case BlendMode::Equation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
			case BlendMode::Equation::Min: return GL_MIN;
			case BlendMode::Equation::Max: return GL_MAX;
			default: return GL_INVALID_ENUM;
		}
	}

	std::unique_ptr<VertexRenderer> VertexRenderer::Create(const size_t maxVertices)
	{
		// Create position vertex buffer object (VBO)
		GLuint positionVbo = 0;
		glCreateBuffers(1, &positionVbo);
		glNamedBufferStorage(positionVbo, maxVertices * sizeof(Math::Float2), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Create texture coordinate vertex buffer object (VBO)
		GLuint textureCoordVbo = 0;
		glCreateBuffers(1, &textureCoordVbo);
		glNamedBufferStorage(textureCoordVbo, maxVertices * sizeof(Math::Float2), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Create index buffer object (EBO/IBO)
		GLuint elementBuffer = 0;
		glCreateBuffers(1, &elementBuffer);
		glNamedBufferStorage(elementBuffer, maxVertices * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Create vertex array object (VAO)
		GLuint vertexArrayId = 0;
		glCreateVertexArrays(1, &vertexArrayId);

		// Attach the element buffer and vertex buffer to the VAO
		glVertexArrayElementBuffer(vertexArrayId, elementBuffer);
		glVertexArrayVertexBuffer(vertexArrayId, 0, positionVbo, 0, sizeof(Math::Float2));
		glVertexArrayVertexBuffer(vertexArrayId, 1, textureCoordVbo, 0, sizeof(Math::Float2));

		// Define the vertex attribute layout (position)
		glEnableVertexArrayAttrib(vertexArrayId, 0);
		glVertexArrayAttribFormat(vertexArrayId, 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vertexArrayId, 0, 0);

		// Define the vertex attribute layout (texture coordinates)
		glEnableVertexArrayAttrib(vertexArrayId, 1);
		glVertexArrayAttribFormat(vertexArrayId, 1, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(vertexArrayId, 1, 1);

		return std::unique_ptr<VertexRenderer>(new VertexRenderer(vertexArrayId, positionVbo, textureCoordVbo, elementBuffer));
	}

	VertexRenderer::~VertexRenderer()
	{
		if (m_VertexArrayId != 0) glDeleteVertexArrays(1, &m_VertexArrayId);
		if (m_VertexBufferId != 0) glDeleteBuffers(1, &m_VertexBufferId);
		if (m_TextureBufferId != 0) glDeleteBuffers(1, &m_TextureBufferId);
		if (m_IndexBufferId != 0) glDeleteBuffers(1, &m_IndexBufferId);
	}

	void VertexRenderer::Render(const Vertices& vertices, const BlendMode& blendMode)
	{
		Activate(blendMode);

		// TODO(Felix): What's up with buffer overflows?

		// Submit the vertex and index data to the GPU
		glNamedBufferSubData(m_VertexBufferId, 0, vertices.Positions.size() * sizeof(Math::Float2), vertices.Positions.data());
		glNamedBufferSubData(m_TextureBufferId, 0, vertices.TexCoords.size() * sizeof(Math::Float2), vertices.TexCoords.data());
		glNamedBufferSubData(m_IndexBufferId, 0, vertices.Indices.size() * sizeof(uint32_t), vertices.Indices.data());

		// Bind the VAO and draw the elements
		glBindVertexArray(m_VertexArrayId);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(vertices.Indices.size()), GL_UNSIGNED_INT, nullptr);
	}

	VertexRenderer::VertexRenderer(const GLuint vertexArrayId, const GLuint vertexBufferId, const GLuint textureBufferId, const GLuint indexBufferId):
		m_VertexArrayId(vertexArrayId),
		m_VertexBufferId(vertexBufferId),
		m_TextureBufferId(textureBufferId),
		m_IndexBufferId(indexBufferId)
	{
	}

	void VertexRenderer::Activate(const BlendMode& blendMode)
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(
			BlendFactorToGlId(blendMode.SourceFactorRGB),
			BlendFactorToGlId(blendMode.DestinationFactorRGB),
			BlendFactorToGlId(blendMode.SourceFactorAlpha),
			BlendFactorToGlId(blendMode.DestinationFactorAlpha)
		);
		glBlendEquationSeparate(
			BlendEquationToGlId(blendMode.BlendEquationRGB),
			BlendEquationToGlId(blendMode.BlendEquationAlpha)
		);
	}
}