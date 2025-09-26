module;

#include <glad/gl.h>
#include <string_view>
#include <stdexcept>
#include <format>

module DGL;

namespace DGL
{
	static constexpr GLenum ShaderTypeToGlId(const Shader::Type type)
	{
		switch (type)
		{
			using enum Shader::Type;
			case Vertex: return GL_VERTEX_SHADER;
			case Fragment: return GL_FRAGMENT_SHADER;
			default: throw std::invalid_argument("Unknown ShaderType");
		}
	}

	std::unique_ptr<Shader> Shader::CreateFromSource(const std::string_view source, const Type type)
	{
		const GLchar* glSource = source.data();

		// Create and compile the shader
		const GLuint shaderId = glCreateShader(ShaderTypeToGlId(type));
		glShaderSource(shaderId, 1, &glSource, nullptr);
		glCompileShader(shaderId);

		GLint compileStatus = GL_FALSE;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus != GL_TRUE)
		{
			// Get the length of the info log
			GLint logLength = 0;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

			// Write the info log to a string and log it
			std::string infoLog(logLength, '\0');
			glGetShaderInfoLog(shaderId, logLength, nullptr, infoLog.data());
			Error(std::format("Failed to compile shader: {}", infoLog));

			// Cleanup
			glDeleteShader(shaderId);
			return nullptr;
		}

		return std::unique_ptr<Shader>(new Shader(shaderId, type));
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

	GLuint Shader::GetShaderId() const
	{
		return m_ShaderId;
	}

	Shader::Type Shader::GetType() const
	{
		return m_Type;
	}

	Shader::Shader(const GLuint shaderId, const Type type) :
		m_ShaderId(shaderId),
		m_Type(type)
	{
	}
}