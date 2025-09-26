module;

#include <memory>

module DGL;

inline constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord; //!< Unused in this one

void main() {
	gl_Position = vec4(a_Position, 0.0, 1.0);
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
	std::unique_ptr<SolidColorBrush> SolidColorBrush::Create(const Color color)
	{
		const auto vertexShader = Shader::CreateFromSource(VERTEX_SOURCE, Shader::Type::Vertex);
		if (vertexShader == nullptr)
		{
			Error("Failed to create vertex shader for solid color brush");
			return nullptr;
		}

		const auto fragmentShader = Shader::CreateFromSource(FRAGMENT_SOURCE, Shader::Type::Fragment);
		if (fragmentShader == nullptr)
		{
			Error("Failed to create fragment shader for solid color brush");
			return nullptr;
		}

		auto shaderProgram = ShaderProgram::Create(*vertexShader, *fragmentShader);
		if (shaderProgram == nullptr)
		{
			Error("Failed to create shader program for solid color brush");
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


	void SolidColorBrush::Apply()
	{
		const auto r = static_cast<float>(m_Color.R) / 255.0f;
		const auto g = static_cast<float>(m_Color.G) / 255.0f;
		const auto b = static_cast<float>(m_Color.B) / 255.0f;
		const auto a = static_cast<float>(m_Color.A) / 255.0f;

		m_Shader->Bind();
		m_Shader->UploadFloat4("u_Color", r, g, b, a);
	}

	SolidColorBrush::SolidColorBrush(std::unique_ptr<ShaderProgram> shaderProgram, const Color color) :
		m_Shader(std::move(shaderProgram)),
		m_Color(color)
	{
	}

}