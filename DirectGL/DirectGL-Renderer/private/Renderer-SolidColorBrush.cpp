module;

#include <glad/gl.h>

module DirectGL.Renderer;

inline constexpr auto VERTEX_SOURCE = R"(
#version 460 core

layout (location = 0) in vec2 a_Position;

uniform mat4 u_ProjectionViewMatrix;

void main()
{
	gl_Position = u_ProjectionViewMatrix * vec4(a_Position, 0.0, 1.0);
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
		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader);

		const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
		glCompileShader(fragmentShader);

		const GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		glValidateProgram(shaderProgram);
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		const GLint colorUniformLocation = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "u_Color");
		if (colorUniformLocation == -1)
		{
			glDeleteProgram(shaderProgram);
			return nullptr;
		}

		const GLint projectionViewMatrixLocation = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "u_ProjectionViewMatrix");
		if (projectionViewMatrixLocation == -1)
		{
			glDeleteProgram(shaderProgram);
			return nullptr;
		}

		return std::unique_ptr<SolidColorBrush>(new SolidColorBrush(shaderProgram, colorUniformLocation, projectionViewMatrixLocation, color));
	}

	SolidColorBrush::~SolidColorBrush()
	{
		if (m_ShaderProgramId)
		{
			glDeleteProgram(m_ShaderProgramId);
		}
	}

	void SolidColorBrush::SetColor(const Color color)
	{
		m_Color = color;
	}

	Color SolidColorBrush::GetColor() const
	{
		return m_Color;
	}

	void SolidColorBrush::UploadUniforms(const Math::Matrix4x4& projectionViewMatrix)
	{
		const float red = static_cast<float>(m_Color.R) / 255.0f;
		const float green = static_cast<float>(m_Color.G) / 255.0f;
		const float blue = static_cast<float>(m_Color.B) / 255.0f;
		const float alpha = static_cast<float>(m_Color.A) / 255.0f;

		// Upload the uniforms by directly setting them in the shader program
		glProgramUniform4f(m_ShaderProgramId, m_ColorLocation, red, green, blue, alpha);
		glProgramUniformMatrix4fv(m_ShaderProgramId, m_ProjectionViewMatrixLocation, 1, GL_FALSE, projectionViewMatrix.GetData());

		// Bind the shader program for rendering
		glUseProgram(m_ShaderProgramId);
	}

	SolidColorBrush::SolidColorBrush(const GLuint shaderProgramId, const GLint colorLocation, const GLint projectionViewMatrixLocation, const Color color) :
		m_Color(color),
		m_ShaderProgramId(shaderProgramId),
		m_ColorLocation(colorLocation),
		m_ProjectionViewMatrixLocation(projectionViewMatrixLocation)
	{
	}
}