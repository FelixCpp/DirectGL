module;

#include <glad/gl.h>

#include <format>

module DirectGL.Brushes;

import DirectGL.Logging;
import Preconditions;

import :Shader;

namespace DGL::Brushes
{
	constexpr GLenum GetShaderTypeId(const ShaderType type)
	{
		switch (type)
		{
			case ShaderType::Vertex: return GL_VERTEX_SHADER;
			case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
			case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
			default: System::Error("Unknown Shader-Type");
		}
	}

	std::unique_ptr<Shader> Shader::Create(const std::string_view source, const ShaderType type)
	{
		const GLuint shaderId = glCreateShader(GetShaderTypeId(type));
		if (shaderId == 0)
		{
			Logging::Error("Failed to create shader");
			return nullptr;
		}

		const char* sourcePtr = source.data();
		glShaderSource(shaderId, 1, &sourcePtr, nullptr);
		glCompileShader(shaderId);

		{
			GLint compilationSucceeded = GL_FALSE;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationSucceeded);
			if (compilationSucceeded != GL_TRUE)
			{
				GLint logLength = 0;
				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

				std::string log(static_cast<size_t>(logLength), '\0');
				glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());

				Logging::Error(std::format("Shader compilation failed: {}", log));

				glDeleteShader(shaderId);
				return nullptr;
			}
		}

		return std::unique_ptr<Shader>(new Shader(shaderId, type));
	}

	Shader::~Shader()
	{
		if (m_RendererId != 0)
		{
			glDeleteShader(m_RendererId);
		}
	}

	ShaderType Shader::GetType() const
	{
		return m_Type;
	}

	GLuint Shader::GetRendererId() const
	{
		return m_RendererId;
	}

	Shader::Shader(const GLuint rendererId, const ShaderType type):
		m_RendererId(rendererId),
		m_Type(type)
	{
	}
}