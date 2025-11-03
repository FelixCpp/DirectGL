module;

#include <glad/gl.h>
#include <ranges>
#include <memory>

module DirectGL;

import :BatchedQuadRenderer;

static constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec3 a_WorldPosition;
layout (location = 1) in vec2 a_LocalPosition;
layout (location = 2) in vec4 a_FillColor;
layout (location = 3) in vec4 a_StrokeColor;
layout (location = 4) in vec2 a_Size;
layout (location = 5) in float a_StrokeWeight;
layout (location = 6) in float a_BorderRadius;

uniform mat4 u_ProjectionViewMatrix;

layout (location = 0) out Output {
	vec2 Size;
	vec2 LocalPosition;
	vec4 FillColor;
	vec4 StrokeColor;
	float StrokeWeight;
	float BorderRadius;
} v_Output;

void main()
{
	gl_Position = u_ProjectionViewMatrix * vec4(a_WorldPosition, 1.0);
	v_Output.Size = a_Size;
	v_Output.LocalPosition = a_LocalPosition;
	v_Output.FillColor = a_FillColor;
	v_Output.StrokeColor = a_StrokeColor;
	v_Output.StrokeWeight = a_StrokeWeight;
	v_Output.BorderRadius = a_BorderRadius;
}
)";

static constexpr auto FRAGMENT_SOURCE = R"(
#version 460 core

#define FADE 1.0

layout (location = 0) out vec4 o_FragColor;

layout (location = 0) in Output {
	vec2 Size;
	vec2 LocalPosition;
	vec4 FillColor;
	vec4 StrokeColor;
	float StrokeWeight;
	float BorderRadius;
} v_Input;

float sdRoundedBox(in vec2 p, in vec2 b, in float r) {
    vec2 q = abs(p)-b+r;
    return min(max(q.x,q.y),0.0) + length(max(q,0.0)) - r;
}

void main()
{
	vec2 uv = v_Input.LocalPosition * v_Input.Size;
	vec2 halfSize = v_Input.Size * 0.5;
	float sdf = sdRoundedBox(uv - halfSize, halfSize, v_Input.BorderRadius);

	float innerStrokeFade = smoothstep(-v_Input.StrokeWeight, -v_Input.StrokeWeight + FADE, sdf);
	float outerStrokeFade = smoothstep(-FADE, 0.0, sdf);
	float strokeFade = innerStrokeFade * (1.0 - outerStrokeFade);
	float fillFade = 1.0 - innerStrokeFade;

	vec4 fillColor = v_Input.FillColor * fillFade;
	vec4 strokeColor = v_Input.StrokeColor * strokeFade;

	o_FragColor = fillColor + strokeColor;
}
)";

namespace DGL
{
	BatchedQuadRenderer::BatchedQuadRenderer(const size_t maxQuadsPerBatch) :
		m_MaxQuadsPerBatch(maxQuadsPerBatch),
		m_QuadsInCurrentBatch(0),
		m_VertexArray(0),
		m_VertexBuffer(0),
		m_ElementBuffer(0),
		m_ShaderProgramId(0),
		m_ProjectionViewMatrixLocation(0),
		m_BatchedVertices(std::make_unique<QuadVertex[]>(maxQuadsPerBatch * 4))
	{
		const auto indices = std::make_unique<uint32_t[]>(maxQuadsPerBatch * 6);
		for (size_t i = 0; i < maxQuadsPerBatch; ++i)
		{
			const size_t baseVertex = i * 4;
			const size_t baseIndex = i * 6;
			indices[baseIndex + 0] = static_cast<uint32_t>(baseVertex + 0);
			indices[baseIndex + 1] = static_cast<uint32_t>(baseVertex + 1);
			indices[baseIndex + 2] = static_cast<uint32_t>(baseVertex + 2);
			indices[baseIndex + 3] = static_cast<uint32_t>(baseVertex + 2);
			indices[baseIndex + 4] = static_cast<uint32_t>(baseVertex + 3);
			indices[baseIndex + 5] = static_cast<uint32_t>(baseVertex + 0);
		}

		glCreateBuffers(1, &m_ElementBuffer);
		glNamedBufferStorage(m_ElementBuffer, maxQuadsPerBatch * 6 * sizeof(uint32_t), indices.get(), GL_DYNAMIC_STORAGE_BIT);

		glCreateBuffers(1, &m_VertexBuffer);
		glNamedBufferStorage(m_VertexBuffer, maxQuadsPerBatch * 4 * sizeof(QuadVertex), nullptr, GL_DYNAMIC_STORAGE_BIT);

		glCreateVertexArrays(1, &m_VertexArray);
		glVertexArrayElementBuffer(m_VertexArray, m_ElementBuffer);
		glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, sizeof(QuadVertex));

		int attribIndex = 0;
		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 3, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, WorldPosition));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, LocalPosition));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 4, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, FillColor));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 4, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, StrokeColor));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 2, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, Size));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 1, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, StrokeWeight));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		glEnableVertexArrayAttrib(m_VertexArray, attribIndex);
		glVertexArrayAttribFormat(m_VertexArray, attribIndex, 1, GL_FLOAT, GL_FALSE, offsetof(QuadVertex, BorderRadius));
		glVertexArrayAttribBinding(m_VertexArray, attribIndex, 0);
		++attribIndex;

		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader);

		const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
		glCompileShader(fragmentShader);

		m_ShaderProgramId = glCreateProgram();
		glAttachShader(m_ShaderProgramId, vertexShader);
		glAttachShader(m_ShaderProgramId, fragmentShader);
		glLinkProgram(m_ShaderProgramId);
		glDetachShader(m_ShaderProgramId, vertexShader);
		glDetachShader(m_ShaderProgramId, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		m_ProjectionViewMatrixLocation = glGetProgramResourceLocation(m_ShaderProgramId, GL_UNIFORM, "u_ProjectionViewMatrix");
	}

	BatchedQuadRenderer::~BatchedQuadRenderer()
	{
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_ElementBuffer);
		glDeleteVertexArrays(1, &m_VertexArray);
		glDeleteProgram(m_ShaderProgramId);
	}

	void BatchedQuadRenderer::BeginDraw(const Math::Matrix4x4& projectionViewMatrix)
	{
		m_QuadsInCurrentBatch = 0;

		glProgramUniformMatrix4fv(m_ShaderProgramId, m_ProjectionViewMatrixLocation, 1, GL_FALSE, projectionViewMatrix.GetData());
	}

	void BatchedQuadRenderer::EndDraw()
	{
		// Only flush the buffer if there is something to flush
		if (m_QuadsInCurrentBatch > 0)
		{
			Flush();
		}
	}

	void BatchedQuadRenderer::SubmitQuad(const QuadProperties& properties)
	{
		// Check if we need to flush the current batch
		if (m_QuadsInCurrentBatch >= m_MaxQuadsPerBatch)
		{
			Flush();
		}

		const size_t baseIndex = m_QuadsInCurrentBatch * 4;
		std::ranges::copy(properties.Vertices, m_BatchedVertices.get() + baseIndex);

		++m_QuadsInCurrentBatch;
	}

	void BatchedQuadRenderer::Flush()
	{
		// Upload batched data to the GPU
		glNamedBufferSubData(m_VertexBuffer, 0, m_QuadsInCurrentBatch * 4 * sizeof(QuadVertex), m_BatchedVertices.get());

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(m_ShaderProgramId);
		glBindVertexArray(m_VertexArray);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_QuadsInCurrentBatch * 6), GL_UNSIGNED_INT, nullptr);

		m_QuadsInCurrentBatch = 0;
	}
}