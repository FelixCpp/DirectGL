module;

#include <glad/gl.h>

#include <format>

module DGL;

namespace DGL
{
	ShaderProgram::ShaderProgram():
		m_ShaderProgramId(0)
	{
	}

	ShaderProgram::~ShaderProgram()
	{
		if (m_ShaderProgramId)
		{
			glDeleteProgram(m_ShaderProgramId);
		}
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept:
		m_ShaderProgramId(std::exchange(other.m_ShaderProgramId, 0))
	{
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
	{
		if (this == &other) return *this;

		m_ShaderProgramId = std::exchange(other.m_ShaderProgramId, 0);
		return *this;
	}

	bool ShaderProgram::Load(const Shader& vertexShader, const Shader& fragmentShader)
	{
		Check(vertexShader.GetType() == Shader::Type::Vertex, [] { return "Provided vertex shader is not of type Vertex"; });
		Check(fragmentShader.GetType() == Shader::Type::Fragment, [] { return "Provided fragment shader is not of type Fragment"; });

		if (m_ShaderProgramId != 0)
		{
			glDeleteProgram(m_ShaderProgramId);
		}

		m_ShaderProgramId = glCreateProgram();
		glAttachShader(m_ShaderProgramId, vertexShader.GetShaderId());
		glAttachShader(m_ShaderProgramId, fragmentShader.GetShaderId());
		glLinkProgram(m_ShaderProgramId);

		GLint linkStatus = GL_FALSE;
		glGetProgramiv(m_ShaderProgramId, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint infoLogLength;
			glGetProgramiv(m_ShaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::string infoLog(infoLogLength, '\0');
			glGetProgramInfoLog(m_ShaderProgramId, infoLogLength, nullptr, infoLog.data());
			Error("Failed to link shader program: " + infoLog);
			glDeleteProgram(m_ShaderProgramId);
			glDetachShader(m_ShaderProgramId, vertexShader.GetShaderId());
			glDetachShader(m_ShaderProgramId, fragmentShader.GetShaderId());
			m_ShaderProgramId = 0;
			return false;
		}

		glValidateProgram(m_ShaderProgramId);

		GLint validateStatus = GL_FALSE;
		glGetProgramiv(m_ShaderProgramId, GL_VALIDATE_STATUS, &validateStatus);
		if (validateStatus != GL_TRUE)
		{
			GLint infoLogLength;
			glGetProgramiv(m_ShaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);
			std::string infoLog(infoLogLength, '\0');
			glGetProgramInfoLog(m_ShaderProgramId, infoLogLength, nullptr, infoLog.data());
			Error("Failed to validate shader program: " + infoLog);
			glDeleteProgram(m_ShaderProgramId);
			glDetachShader(m_ShaderProgramId, vertexShader.GetShaderId());
			glDetachShader(m_ShaderProgramId, fragmentShader.GetShaderId());
			m_ShaderProgramId = 0;
			return false;
		}

		glDetachShader(m_ShaderProgramId, vertexShader.GetShaderId());
		glDetachShader(m_ShaderProgramId, fragmentShader.GetShaderId());
		return true;
	}

	void ShaderProgram::Bind()
	{
		glUseProgram(m_ShaderProgramId);
	}

	void ShaderProgram::UploadInt1(const std::string_view name, const std::int32_t x)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1i(m_ShaderProgramId, GetUniformLocation(name), x);
		}
	}

	void ShaderProgram::UploadFloat1Array(const std::string_view name, const float* data, const std::size_t count)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1fv(m_ShaderProgramId, GetUniformLocation(name), static_cast<GLsizei>(count), data);
		}
	}

	void ShaderProgram::UploadFloat4Array(const std::string_view name, const float* data, const std::size_t count)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4fv(m_ShaderProgramId, GetUniformLocation(name), static_cast<GLsizei>(count), data);
		}
	}

	void ShaderProgram::UploadFloat2(const std::string_view name, const float x, const float y)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2f(m_ShaderProgramId, GetUniformLocation(name), x, y);
		}
	}

	void ShaderProgram::UploadFloat4(const std::string_view name, const float x, const float y, const float z, const float w)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4f(m_ShaderProgramId, GetUniformLocation(name), x, y, z, w);
		}
	}

	GLint ShaderProgram::GetUniformLocation(std::string_view name)
	{
		const auto itr = m_UniformLocationCache.find(std::string(name));
		if (itr != m_UniformLocationCache.end())
		{
			return itr->second;
		}

		GLint location = glGetUniformLocation(m_ShaderProgramId, std::string(name).c_str());
		if (location == -1)
		{
			Warning(std::format("Uniform '{}' not found in shader program.", name));
		}

		m_UniformLocationCache.emplace(std::string(name), location);
		return location;
	}

}