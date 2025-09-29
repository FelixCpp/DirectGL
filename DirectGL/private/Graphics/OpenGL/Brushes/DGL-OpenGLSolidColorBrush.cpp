module;

#include <glad/gl.h>

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
		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		if (vertexShader == 0)
		{
			Error("Failed to create vertex shader.");
			return nullptr;
		}

		glShaderSource(vertexShader, 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader);

		{
			GLint success = GL_FALSE;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (success != GL_TRUE)
			{
				GLint logLength = 0;
				glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

				std::string infoLog(static_cast<size_t>(logLength), '\0');
				glGetShaderInfoLog(vertexShader, logLength, nullptr, infoLog.data());

				glDeleteShader(vertexShader);

				Error("Failed to compile vertex shader. Info log: " + infoLog);
				return nullptr;
			}
		}

		const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		if (fragmentShader == 0)
		{
			glDeleteShader(vertexShader);
			Error("Failed to create fragment shader.");
			return nullptr;
		}

		glShaderSource(fragmentShader, 1, &FRAGMENT_SOURCE, nullptr);
		glCompileShader(fragmentShader);

		{
			GLint success = GL_FALSE;
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (success != GL_TRUE)
			{
				GLint logLength = 0;
				glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

				std::string infoLog(static_cast<size_t>(logLength), '\0');
				glGetShaderInfoLog(fragmentShader, logLength, nullptr, infoLog.data());

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);

				Error("Failed to compile fragment shader. Info log: " + infoLog);
				return nullptr;
			}
		}

		const GLuint shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		{
			GLint success = GL_FALSE;
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (success != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

				std::string infoLog(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());

				glDetachShader(shaderProgram, vertexShader);
				glDetachShader(shaderProgram, fragmentShader);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				glDeleteProgram(shaderProgram);

				Error("Failed to link shader program. Info log: " + infoLog);
				return nullptr;
			}
		}

		glValidateProgram(shaderProgram);

		{
			GLint success = GL_FALSE;
			glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
			if (success != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

				std::string infoLog(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());

				glDetachShader(shaderProgram, vertexShader);
				glDetachShader(shaderProgram, fragmentShader);

				glDeleteShader(vertexShader);
				glDeleteShader(fragmentShader);
				glDeleteProgram(shaderProgram);

				Error("Failed to validate shader program. Info log: " + infoLog);
				return nullptr;
			}
		}

		const GLint colorLocation = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "u_Color");
		if (colorLocation == -1)
		{
			glDetachShader(shaderProgram, vertexShader);
			glDetachShader(shaderProgram, fragmentShader);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(shaderProgram);

			Error("Failed to find uniform 'u_Color' in shader program.");
			return nullptr;
		}

		const GLint viewProjectionLocation = glGetProgramResourceLocation(shaderProgram, GL_UNIFORM, "u_ViewProjection");
		if (viewProjectionLocation == -1)
		{
			glDetachShader(shaderProgram, vertexShader);
			glDetachShader(shaderProgram, fragmentShader);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glDeleteProgram(shaderProgram);
			Error("Failed to find uniform 'u_ViewProjection' in shader program.");
			return nullptr;
		}

		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return std::unique_ptr<OpenGLSolidColorBrush>(new OpenGLSolidColorBrush(color, shaderProgram, colorLocation, viewProjectionLocation));
	}

	OpenGLSolidColorBrush::~OpenGLSolidColorBrush()
	{
		glDeleteProgram(m_ShaderProgramId);
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

		glUseProgram(m_ShaderProgramId);
		glProgramUniform4f(m_ShaderProgramId, m_ColorUniformLocation, red, green, blue, alpha);
		glProgramUniformMatrix4fv(m_ShaderProgramId, m_ViewProjectionUniformLocation, 1, GL_FALSE, camera.GetViewProjectionMatrix().GetData());
	}

	OpenGLSolidColorBrush::OpenGLSolidColorBrush(const Color color, const GLuint shaderProgramId, const GLint colorUniformLocation, const GLint viewProjectionUniformLocation):
		m_Color(color), m_ShaderProgramId(shaderProgramId), m_ColorUniformLocation(colorUniformLocation), m_ViewProjectionUniformLocation(viewProjectionUniformLocation)
	{
	}
}