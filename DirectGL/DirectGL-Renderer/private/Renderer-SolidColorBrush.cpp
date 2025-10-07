module;

#include <glad/gl.h>
#include <format>

module DirectGL.Renderer;

import Preconditions;

import :Logging;
import :AutoRelease;

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
		const AutoRelease<GLuint> vertexShader(glCreateShader(GL_VERTEX_SHADER), [](const GLuint value) { glDeleteShader(value); });
		glShaderSource(vertexShader.Get(), 1, &VERTEX_SOURCE, nullptr);
		glCompileShader(vertexShader.Get());
		{
			GLint compilationSucceeded = GL_FALSE;
			glGetShaderiv(vertexShader.Get(), GL_COMPILE_STATUS, &compilationSucceeded);
			if (compilationSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetShaderiv(vertexShader.Get(), GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetShaderInfoLog(vertexShader.Get(), logLength, nullptr, log.data());

				Error(std::format("Vertex shader compilation failed: {}", log));
				return nullptr;
			}
		}

		const AutoRelease<GLuint> fragmentShader(glCreateShader(GL_FRAGMENT_SHADER), [](const GLuint value) { glDeleteShader(value); });
		glShaderSource(fragmentShader.Get(), 1, &FRAGMENT_SOURCE, nullptr);
		glCompileShader(fragmentShader.Get());
		{
			GLint compilationSucceeded = GL_FALSE;
			glGetShaderiv(fragmentShader.Get(), GL_COMPILE_STATUS, &compilationSucceeded);
			if (compilationSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetShaderiv(fragmentShader.Get(), GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetShaderInfoLog(fragmentShader.Get(), logLength, nullptr, log.data());

				Error(std::format("Fragment shader compilation failed: {}", log));
				return nullptr;
			}
		}

		AutoRelease<GLuint> shaderProgram(glCreateProgram(), [](const GLuint value) { glDeleteProgram(value); });
		glAttachShader(shaderProgram.Get(), vertexShader.Get());
		glAttachShader(shaderProgram.Get(), fragmentShader.Get());

		const auto _releaseVs = System::Finally([program = shaderProgram.Get(), vs = vertexShader.Get()] { glDetachShader(program, vs); });
		const auto _releaseFs = System::Finally([program = shaderProgram.Get(), fs = fragmentShader.Get()] { glDetachShader(program, fs); });

		glLinkProgram(shaderProgram.Get());
		{
			GLint linkageSucceeded = GL_FALSE;
			glGetProgramiv(shaderProgram.Get(), GL_LINK_STATUS, &linkageSucceeded);
			if (linkageSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgram.Get(), GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgram.Get(), logLength, nullptr, log.data());

				Error(std::format("Shader program linkage failed: {}", log));
				return nullptr;
			}
		}

		glValidateProgram(shaderProgram.Get());
		{
			GLint validationSucceeded = GL_FALSE;
			glGetProgramiv(shaderProgram.Get(), GL_VALIDATE_STATUS, &validationSucceeded);
			if (validationSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgram.Get(), GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgram.Get(), logLength, nullptr, log.data());

				Error(std::format("Shader program validation failed: {}", log));
				return nullptr;
			}
		}

		const GLint colorUniformLocation = glGetProgramResourceLocation(shaderProgram.Get(), GL_UNIFORM, "u_Color");
		if (colorUniformLocation == -1)
		{
			Warning("Couldn't find color uniform in solid color brush shader");
			return nullptr;
		}

		const GLint projectionViewMatrixLocation = glGetProgramResourceLocation(shaderProgram.Get(), GL_UNIFORM, "u_ProjectionViewMatrix");
		if (projectionViewMatrixLocation == -1)
		{
			Warning("Couldn't find projection-view matrix uniform in solid color brush shader");
			return nullptr;
		}

		return std::unique_ptr<SolidColorBrush>(new SolidColorBrush(std::move(shaderProgram), colorUniformLocation, projectionViewMatrixLocation, color));
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
		glProgramUniform4f(m_ShaderProgramId.Get(), m_ColorLocation, red, green, blue, alpha);
		glProgramUniformMatrix4fv(m_ShaderProgramId.Get(), m_ProjectionViewMatrixLocation, 1, GL_FALSE, projectionViewMatrix.GetData());

		// Bind the shader program for rendering
		glUseProgram(m_ShaderProgramId.Get());
	}

	SolidColorBrush::SolidColorBrush(AutoRelease<GLuint>&& shaderProgramId, const GLint colorLocation, const GLint projectionViewMatrixLocation, const Color color) :
		m_Color(color),
		m_ShaderProgramId(std::move(shaderProgramId)),
		m_ColorLocation(colorLocation),
		m_ProjectionViewMatrixLocation(projectionViewMatrixLocation)
	{
	}
}