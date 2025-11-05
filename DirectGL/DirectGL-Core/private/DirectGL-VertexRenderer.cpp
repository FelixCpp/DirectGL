module;

#include <glad/gl.h>

#include <ranges>
#include <algorithm>
#include <format>

module DirectGL;

import DirectGL.Logging;

import :VertexRenderer;

static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;

layout (location = 0) out vec4 v_Color;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
	v_Color = a_Color;
}
)";

static constexpr auto FRAMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 0) in vec4 v_Color;

void main()
{
	o_FragColor = v_Color;
}
)";

namespace DGL
{
	VertexRenderer::VertexRenderer() :
		m_VertexArray(0),
		m_PositionBuffer(0),
		m_ColorBuffer(0),
		m_ElementBuffer(0),
		m_ShaderProgram(0)
	{
		glCreateBuffers(1, &m_PositionBuffer);
		glNamedBufferStorage(m_PositionBuffer, 1024 * sizeof(Math::Float3), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &m_ColorBuffer);
		glNamedBufferStorage(m_ColorBuffer, 1024 * sizeof(Math::Float4), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &m_ElementBuffer);
		glNamedBufferStorage(m_ElementBuffer, 1024 * sizeof(uint32_t), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &m_VertexArray);

		glVertexArrayVertexBuffer(m_VertexArray, 0, m_PositionBuffer, 0, sizeof(Math::Float3));
		glVertexArrayVertexBuffer(m_VertexArray, 1, m_ColorBuffer, 0, sizeof(Math::Float4));
		glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);

		glEnableVertexArrayAttrib(m_VertexArray, 0);
		glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_VertexArray, 0, 0);

		glEnableVertexArrayAttrib(m_VertexArray, 1);
		glVertexArrayAttribFormat(m_VertexArray, 1, 4, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_VertexArray, 1, 1);

		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader);

		const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &FRAMENT_SOURCE, nullptr);
		glCompileShader(fragmentShader);

		m_ShaderProgram = glCreateProgram();
		glAttachShader(m_ShaderProgram, vertexShader);
		glAttachShader(m_ShaderProgram, fragmentShader);
		glLinkProgram(m_ShaderProgram);
		glDetachShader(m_ShaderProgram, vertexShader);
		glDetachShader(m_ShaderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		m_ProjectionViewMatrixLocation = glGetProgramResourceLocation(m_ShaderProgram, GL_UNIFORM, "u_ProjectionViewMatrix");
		m_ModelMatrixLocation = glGetProgramResourceLocation(m_ShaderProgram, GL_UNIFORM, "u_ModelMatrix");
	}

	void VertexRenderer::BeginDraw(const Math::Matrix4x4& projectionViewMatrix)
	{
		glProgramUniformMatrix4fv(m_ShaderProgram, m_ProjectionViewMatrixLocation, 1, GL_FALSE, projectionViewMatrix.GetData());
		m_DrawCalls = 0;
	}

	void VertexRenderer::EndDraw()
	{
		Debug(std::format("VertexRenderer: Draw Calls this frame: {}", m_DrawCalls));
	}

	void VertexRenderer::Submit(const std::span<const Math::Float3>& positions, const std::span<const Math::Float4>& colors, const std::span<const uint32_t>& indices, const Math::Matrix4x4& modelMatrix)
	{
		glNamedBufferSubData(m_PositionBuffer, 0, positions.size_bytes(), positions.data());
		glNamedBufferSubData(m_ColorBuffer, 0, colors.size_bytes(), colors.data());
		glNamedBufferSubData(m_ElementBuffer, 0, indices.size_bytes(), indices.data());

		glUseProgram(m_ShaderProgram);
		glProgramUniformMatrix4fv(m_ShaderProgram, m_ModelMatrixLocation, 1, GL_FALSE, modelMatrix.GetData());

		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);

		++m_DrawCalls;
	}

	void VertexRenderer::Flush()
	{
	}
}