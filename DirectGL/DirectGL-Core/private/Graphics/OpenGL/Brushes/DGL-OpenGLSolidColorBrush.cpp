module;

#include <memory>

module DGL;

inline constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord; //!< Unused in this one

uniform mat4 u_ViewProjection;

void main() {
	gl_Position = u_ViewProjection * vec4(a_Position, 0.0, 1.0);
}
)";

inline constexpr auto FRAGMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;

uniform vec4 u_Color;

void main() {
	o_FragColor = u_Color;
}
)";

namespace DGL
{
	std::unique_ptr<OpenGLSolidColorBrush> OpenGLSolidColorBrush::Create(const Color color)
	{
		const auto vertexShader = OpenGLShader::Create(VERTEX_SOURCE, ShaderType::Vertex);
		if (vertexShader == nullptr)
		{
			Error("Failed to create vertex shader for solid color brush");
			return nullptr;
		}

		const auto fragmentShader = OpenGLShader::Create(FRAGMENT_SOURCE, ShaderType::Fragment);
		if (fragmentShader == nullptr)
		{
			Error("Failed to create fragment shader for solid color brush");
			return nullptr;
		}

		auto shaderProgram = OpenGLShaderProgram::Create(*vertexShader, *fragmentShader);
		if (shaderProgram == nullptr)
		{
			Error("Failed to create shader program for solid color brush");
			return nullptr;
		}

		return std::unique_ptr<OpenGLSolidColorBrush>(new OpenGLSolidColorBrush(std::move(shaderProgram), color));
	}

	void OpenGLSolidColorBrush::SetColor(const Color color)
	{
		m_Color = color;
	}

	Color OpenGLSolidColorBrush::GetColor() const
	{
		return m_Color;
	}

	void OpenGLSolidColorBrush::Apply(const Camera& camera)
	{
		const float red = static_cast<float>(m_Color.R) / 255.0f;
		const float green = static_cast<float>(m_Color.G) / 255.0f;
		const float blue = static_cast<float>(m_Color.B) / 255.0f;
		const float alpha = static_cast<float>(m_Color.A) / 255.0f;

		m_ShaderProgram->Bind();
		m_ShaderProgram->UploadFloat4("u_Color", red, green, blue, alpha);
		m_ShaderProgram->UploadMatrix4x4("u_ViewProjection", std::span<const float, 16>(camera.GetViewProjectionMatrix().GetData(), 16));
	}

	OpenGLSolidColorBrush::OpenGLSolidColorBrush(std::unique_ptr<OpenGLShaderProgram> shaderProgram, const Color color):
		m_Color(color),
		m_ShaderProgram(std::move(shaderProgram))
	{
	}
}