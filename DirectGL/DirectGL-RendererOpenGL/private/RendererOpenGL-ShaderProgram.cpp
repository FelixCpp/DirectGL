module;

#include <glad/gl.h>

#include <format>
#include <string>

module DirectGL.RendererOpenGL;

import :Shader;

namespace DGL::RendererOpenGL
{
	std::unique_ptr<OpenGLShaderProgram> OpenGLShaderProgram::Create(const std::string_view vertexSource, const std::string_view fragmentSource)
	{
		const auto vertexShader = OpenGLShader::Create(vertexSource, ShaderType::Vertex);
		if (vertexShader == nullptr)
		{
			s_Logger.Log(LogLevel::Error, "Failed to create OpenGLShaderProgram because the vertex shader could not have been instantiated.");
			return nullptr;
		}

		const auto fragmentShader = OpenGLShader::Create(fragmentSource, ShaderType::Fragment);
		if (vertexShader == nullptr)
		{
			s_Logger.Log(LogLevel::Error, "Failed to create OpenGLShaderProgram because the fragment shader could not have been instantiated.");
			return nullptr;
		}

		const GLuint shaderProgramId = glCreateProgram();
		glAttachShader(shaderProgramId, vertexShader->GetRendererId());
		glAttachShader(shaderProgramId, fragmentShader->GetRendererId());
		glLinkProgram(shaderProgramId);
		glValidateProgram(shaderProgramId);
		glDetachShader(shaderProgramId, vertexShader->GetRendererId());
		glDetachShader(shaderProgramId, fragmentShader->GetRendererId());

		GLint linkageSucceeded = GL_FALSE;
		glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkageSucceeded);
		if (linkageSucceeded != GL_TRUE)
		{
			GLint infoLogLength = 0;
			glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::string buffer(static_cast<size_t>(infoLogLength), '\0');
			glGetProgramInfoLog(shaderProgramId, buffer.length(), &infoLogLength, buffer.data());

			s_Logger.Log(LogLevel::Error, std::format("Failed to link the shader program: {}", buffer));
			return nullptr;
		}

		GLint validationSucceeded = GL_FALSE;
		glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &validationSucceeded);
		if (linkageSucceeded != GL_TRUE)
		{
			GLint infoLogLength = 0;
			glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::string buffer(static_cast<size_t>(infoLogLength), '\0');
			glGetProgramInfoLog(shaderProgramId, buffer.length(), &infoLogLength, buffer.data());

			s_Logger.Log(LogLevel::Error, std::format("Failed to validate the shader program: {}", buffer));
			return nullptr;
		}

		return std::unique_ptr<OpenGLShaderProgram>(new OpenGLShaderProgram(shaderProgramId));
	}

	GLuint OpenGLShaderProgram::GetRendererId() const
	{
		return m_RendererId;
	}

	OpenGLShaderProgram::OpenGLShaderProgram(const GLuint rendererId) :
		m_RendererId(rendererId)
	{
	}

}