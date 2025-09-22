module DGL;

inline constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord;

layout (location = 0) out vec4 v_Color;

void main() {
	gl_Position = vec4(a_Position, 0.0, 1.0);
	v_Color = vec4(a_TexCoord, 0.0, 1.0);
}
)";

inline constexpr auto FRAGMENT_SOURCE = R"(
#version 460 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 0) in vec4 v_Color;

uniform vec4 u_Color;

void main() {
	o_FragColor = v_Color;
}
)";

namespace DGL
{
	SolidColorBrush::SolidColorBrush(const float red, const float green, const float blue, const float alpha):
		m_Red(red), m_Green(green), m_Blue(blue), m_Alpha(alpha)
	{
		Shader vertexShader;
		if (not vertexShader.LoadFromMemory(VERTEX_SOURCE, Shader::Type::Vertex))
		{
			Error("Failed to load vertex shader for solid color brush.");
		}

		Shader fragmentShader;
		if (not fragmentShader.LoadFromMemory(FRAGMENT_SOURCE, Shader::Type::Fragment))
		{
			Error("Failed to load fragment shader for solid color brush.");
		}

		if (not m_Shader.Load(vertexShader, fragmentShader))
		{
			Error("Failed to create shader program for solid color brush.");
		}
	}

	void SolidColorBrush::SetColor(const float red, const float green, const float blue, const float alpha)
	{
		m_Red = red;
		m_Green = green;
		m_Blue = blue;
		m_Alpha = alpha;
	}

	void SolidColorBrush::Apply()
	{
		m_Shader.Bind();
		m_Shader.UploadFloat4("u_Color", m_Red, m_Green, m_Blue, m_Alpha);
	}
}
