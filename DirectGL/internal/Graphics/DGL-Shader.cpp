module;

#include <glad/gl.h>
#include <string_view>
#include <stdexcept>
#include <format>

module DGL;

namespace DGL
{
	Shader::Shader():
		m_ShaderId(0),
		m_Type()
	{
	}

	Shader::~Shader()
	{
		if (m_ShaderId)
		{
			glDeleteShader(m_ShaderId);
		}
	}

	Shader::Shader(Shader&& other) noexcept:
		m_ShaderId(std::exchange(other.m_ShaderId, 0)),
		m_Type(std::move(other.m_Type))
	{
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ShaderId)
			{
				glDeleteShader(m_ShaderId);
			}

			m_ShaderId = std::exchange(other.m_ShaderId, 0);
			m_Type = std::move(other.m_Type);
		}

		return *this;
	}

	bool Shader::LoadFromMemory(const std::string_view source, const Type type)
	{
		if (m_ShaderId != 0)
		{
			glDeleteShader(m_ShaderId);
		}

		const GLenum shaderType = [type]
		{
			switch (type)
			{
				case Type::Vertex: return GL_VERTEX_SHADER;
				case Type::Fragment: return GL_FRAGMENT_SHADER;
				default: throw std::invalid_argument("Unknown ShaderType");
			}
		}();

		const GLchar* glSource = source.data();

		m_Type = type;
		m_ShaderId = glCreateShader(shaderType);
		glShaderSource(m_ShaderId, 1, &glSource, nullptr);
		glCompileShader(m_ShaderId);

		GLint compileStatus = GL_FALSE;
		glGetShaderiv(m_ShaderId, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus != GL_TRUE)
		{
			GLint logLength = 0;
			glGetShaderiv(m_ShaderId, GL_INFO_LOG_LENGTH, &logLength);

			std::string infoLog(logLength, '\0');
			glGetShaderInfoLog(m_ShaderId, logLength, nullptr, infoLog.data());

			glDeleteShader(m_ShaderId);
			m_ShaderId = 0;

			Error(std::format("Failed to compile shader: {}", infoLog));
			return false;
		}

		return true;
	}

	GLuint Shader::GetShaderId() const
	{
		return m_ShaderId;
	}

	Shader::Type Shader::GetType() const
	{
		return m_Type;
	}
}