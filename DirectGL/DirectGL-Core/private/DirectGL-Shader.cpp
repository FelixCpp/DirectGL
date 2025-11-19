module;

#include <glad/gl.h>
#include <fstream>
#include <sstream>
#include <optional>
#include <memory>
#include <format>

module DirectGL;

import :Shader;

namespace 
{
	std::optional<std::string> LoadFileAsString(const std::filesystem::path& filePath)
	{
		const std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
		if (!fileStream)
		{
			return std::nullopt;
		}

		std::ostringstream ss;
		ss << fileStream.rdbuf();
		return ss.str();
	}

	GLuint CompileShader(const std::string_view source, const GLenum shaderType)
	{
		const char* sourceCStr = source.data();
		const GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &sourceCStr, nullptr);
		glCompileShader(shader);

		GLint compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus != GL_TRUE)
		{
			GLint infoLogLength;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
			glGetShaderInfoLog(shader, infoLogLength, nullptr, infoLog.get());

			glDeleteShader(shader);

			DGL::Error(std::format("Shader compilation failed: {}", infoLog.get()));
			return 0;
		}

		return shader;
	}

	GLuint LinkProgram(const GLuint vertexShader, const GLuint fragmentShader)
	{
		const GLuint program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.get());

			glDetachShader(program, vertexShader);
			glDetachShader(program, fragmentShader);
			glDeleteProgram(program);

			DGL::Error(std::format("Shader program linking failed: {}", infoLog.get()));
			return 0;
		}

		GLint validateStatus;
		glValidateProgram(program);
		glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
		if (validateStatus != GL_TRUE)
		{
			GLint infoLogLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::unique_ptr<GLchar[]> infoLog(new GLchar[infoLogLength]);
			glGetProgramInfoLog(program, infoLogLength, nullptr, infoLog.get());

			glDetachShader(program, vertexShader);
			glDetachShader(program, fragmentShader);
			glDeleteProgram(program);

			DGL::Error(std::format("Shader program validation failed: {}", infoLog.get()));
			return 0;
		}

		return program;
	}
}

namespace DGL
{
	std::unique_ptr<Shader> Shader::CreateFromSource(const std::string_view vertexSource, const std::string_view fragmentSource)
	{
		const GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
		if (vertexShader == 0)
		{
			return nullptr;
		}

		const GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
		if (fragmentShader == 0)
		{
			glDeleteShader(vertexShader);
			return nullptr;
		}

		const GLuint shaderProgram = LinkProgram(vertexShader, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (shaderProgram == 0)
		{
			return nullptr;
		}

		return std::unique_ptr<Shader>(new Shader(shaderProgram));
	}

	std::unique_ptr<Shader> Shader::CreateFromFiles(const std::filesystem::path& vertexFilePath, const std::filesystem::path& fragmentFilePath)
	{
		const auto vertexSourceOpt = LoadFileAsString(vertexFilePath);
		if (not vertexSourceOpt.has_value())
		{
			return nullptr;
		}

		const auto fragmentSourceOpt = LoadFileAsString(fragmentFilePath);
		if (not fragmentSourceOpt.has_value())
		{
			return nullptr;
		}

		return CreateFromSource(vertexSourceOpt.value(), fragmentSourceOpt.value());
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderId);
	}

	void Shader::UploadInt1(const std::string_view name, const int32_t value)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1i(m_ShaderId, location, value);
		}
	}

	void Shader::UploadInt2(const std::string_view name, const int32_t value1, const int32_t value2)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2i(m_ShaderId, location, value1, value2);
		}
	}

	void Shader::UploadInt3(const std::string_view name, const int32_t value1, const int32_t value2, const int32_t value3)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3i(m_ShaderId, location, value1, value2, value3);
		}
	}

	void Shader::UploadInt4(const std::string_view name, const int32_t value1, const int32_t value2, const int32_t value3, const int32_t value4)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4i(m_ShaderId, location, value1, value2, value3, value4);
		}
	}

	void Shader::UploadFloat1(const std::string_view name, const float value)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1f(m_ShaderId, location, value);
		}
	}

	void Shader::UploadFloat2(const std::string_view name, const float value1, const float value2)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2f(m_ShaderId, location, value1, value2);
		}
	}

	void Shader::UploadFloat3(const std::string_view name, const float value1, const float value2, const float value3)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3f(m_ShaderId, location, value1, value2, value3);
		}
	}

	void Shader::UploadFloat4(const std::string_view name, const float value1, const float value2, const float value3, const float value4)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4f(m_ShaderId, location, value1, value2, value3, value4);
		}
	}

	void Shader::UploadFloatMatrix4x4(const std::string_view name, const float* matrixData)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniformMatrix4fv(m_ShaderId, location, 1, GL_FALSE, matrixData);
		}
	}

	uint32_t Shader::GetShaderId() const
	{
		return m_ShaderId;
	}

	int32_t Shader::GetUniformLocation(const std::string_view name)
	{
		const auto itr = m_UniformLocationCache.find(name);
		if (itr != m_UniformLocationCache.end())
		{
			return itr->second;
		}

		const GLint location = glGetUniformLocation(m_ShaderId, name.data());
		if (location == -1)
		{
			Warning(std::format("Uniform '{}' not found in shader program {}", name, m_ShaderId));
		}

		m_UniformLocationCache.emplace(name, location);
		return location;
	}

	Shader::Shader(const uint32_t shaderId) :
		m_ShaderId(shaderId)
	{
	}
}