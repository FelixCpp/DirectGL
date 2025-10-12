module;

#include <glad/gl.h>

#include <string>
#include <format>

module DirectGL.Renderer;

import DirectGL.Logging;

import :Shader;
import :ShaderProgram;

import Preconditions;

namespace DGL::Renderer
{
	std::unique_ptr<ShaderProgram> ShaderProgram::Create(const Shader& vertexShader, const Shader& fragmentShader)
	{
		if (vertexShader.GetType() != ShaderType::Vertex)
		{
			Logging::Error("Provided vertex shader is not of type Vertex");
			return nullptr;
		}

		if (fragmentShader.GetType() != ShaderType::Fragment)
		{
			Logging::Error("Provided fragment shader is not of type Fragment");
			return nullptr;
		}

		const GLuint shaderProgramId = glCreateProgram();
		if (shaderProgramId == 0)
		{
			Logging::Error("Failed to create shader program");
			return nullptr;
		}

		glAttachShader(shaderProgramId, vertexShader.GetRendererId());
		glAttachShader(shaderProgramId, fragmentShader.GetRendererId());

		glLinkProgram(shaderProgramId);
		{
			GLint linkageSucceeded = GL_FALSE;
			glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkageSucceeded);
			if (linkageSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgramId, logLength, nullptr, log.data());

				Logging::Error(std::format("Shader program linkage failed: {}", log));

				glDetachShader(shaderProgramId, vertexShader.GetRendererId());
				glDetachShader(shaderProgramId, fragmentShader.GetRendererId());
				glDeleteProgram(shaderProgramId);
				return nullptr;
			}
		}

		glValidateProgram(shaderProgramId);
		{
			GLint validationSucceeded = GL_FALSE;
			glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &validationSucceeded);
			if (validationSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetProgramInfoLog(shaderProgramId, logLength, nullptr, log.data());

				Logging::Error(std::format("Shader program validation failed: {}", log));

				glDetachShader(shaderProgramId, vertexShader.GetRendererId());
				glDetachShader(shaderProgramId, fragmentShader.GetRendererId());
				glDeleteProgram(shaderProgramId);
				return nullptr;
			}
		}

		glDetachShader(shaderProgramId, vertexShader.GetRendererId());
		glDetachShader(shaderProgramId, fragmentShader.GetRendererId());

		return std::unique_ptr<ShaderProgram>(new ShaderProgram(shaderProgramId));
	}

	ShaderProgram::~ShaderProgram()
	{
		if (m_RendererId != 0)
		{
			glDeleteProgram(m_RendererId);
		}
	}

	void ShaderProgram::UploadTexture(const std::string_view name, const GLuint textureUnit)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1i(m_RendererId, location, static_cast<GLint>(textureUnit));
		}
	}

	void ShaderProgram::UploadFloat1(const std::string_view name, const float x)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1f(m_RendererId, location, x);
		}
	}

	void ShaderProgram::UploadFloat2(const std::string_view name, const float x, const float y)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2f(m_RendererId, location, x, y);
		}
	}

	void ShaderProgram::UploadFloat3(const std::string_view name, const float x, const float y, const float z)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3f(m_RendererId, location, x, y, z);
		}
	}

	void ShaderProgram::UploadFloat4(const std::string_view name, const float x, const float y, const float z, const float w)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4f(m_RendererId, location, x, y, z, w);
		}
	}

	void ShaderProgram::UploadMatrix4x4(const std::string_view name, const std::span<const float, 16>& matrix)
	{
		if (const auto location = GetUniformLocation(name); location != -1)
		{
			glProgramUniformMatrix4fv(m_RendererId, location, 1, GL_FALSE, matrix.data());
		}
	}

	GLuint ShaderProgram::GetRendererId() const
	{
		return m_RendererId;
	}

	void ShaderProgram::Activate(const ShaderProgram* program)
	{
		glUseProgram(program != nullptr ? program->GetRendererId() : 0);
	}

	ShaderProgram::ShaderProgram(const GLuint rendererId):
		m_RendererId(rendererId)
	{
	}

	GLint ShaderProgram::GetUniformLocation(const std::string_view name)
	{
		const auto itr = m_UniformLocationCache.find(name);
		if (itr != m_UniformLocationCache.end())
		{
			return itr->second;
		}

		GLint location = glGetProgramResourceLocation(m_RendererId, GL_UNIFORM, name.data());
		if (location == -1)
		{
			Logging::Warning(std::format("Uniform '{}' not found in shader program", name));
		}

		m_UniformLocationCache.emplace(name, location);
		return location;
	}
}