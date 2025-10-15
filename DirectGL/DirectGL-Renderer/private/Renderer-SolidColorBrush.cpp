module;

#include <glad/gl.h>
#include <format>

module DirectGL.Renderer;
import DirectGL.Logging;

inline constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);
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

namespace DGL::Renderer
{
	std::unique_ptr<SolidColorBrush> SolidColorBrush::Create(const Color color)
	{
		const auto vertexShader = Shader::Create(VERTEX_SOURCE, ShaderType::Vertex);
		if (vertexShader == nullptr)
		{
			Logging::Error("Failed to create vertex shader");
			return nullptr;
		}

		const auto fragmentShader = Shader::Create(FRAGMENT_SOURCE, ShaderType::Fragment);
		if (fragmentShader == nullptr)
		{
			Logging::Error("Failed to create fragment shader");
			return nullptr;
		}

		auto shaderProgram = ShaderProgram::Create(*vertexShader, *fragmentShader);
		if (shaderProgram == nullptr)
		{
			Logging::Error("Failed to create shader program");
			return nullptr;
		}

		return std::unique_ptr<SolidColorBrush>(new SolidColorBrush(std::move(shaderProgram), color));
	}

	void SolidColorBrush::SetColor(const Color color)
	{
		m_Color = color;
	}

	Color SolidColorBrush::GetColor() const
	{
		return m_Color;
	}

	void SolidColorBrush::UploadUniforms(const Math::Matrix4x4& projectionViewMatrix, const Math::Matrix4x4& modelMatrix)
	{
		const float red = static_cast<float>(m_Color.R) / 255.0f;
		const float green = static_cast<float>(m_Color.G) / 255.0f;
		const float blue = static_cast<float>(m_Color.B) / 255.0f;
		const float alpha = static_cast<float>(m_Color.A) / 255.0f;

		// Upload the uniforms by directly setting them in the shader program
		m_ShaderProgram->UploadFloat4("u_Color", red, green, blue, alpha);
		m_ShaderProgram->UploadMatrix4x4("u_ProjectionViewMatrix", std::span<const float, 16>(projectionViewMatrix.GetData(), 16));
		m_ShaderProgram->UploadMatrix4x4("u_ModelMatrix", std::span<const float, 16>(modelMatrix.GetData(), 16));

		// Bind the shader program for rendering
		ShaderProgram::Activate(m_ShaderProgram.get());
	}

	SolidColorBrush::SolidColorBrush(std::unique_ptr<ShaderProgram> shaderProgram, const Color color) :
		m_Color(color),
		m_ShaderProgram(std::move(shaderProgram))
	{
	}
}