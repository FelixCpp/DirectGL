module;

#include <glad/gl.h>

#include <format>

module DGL;

inline static constexpr auto INVALID_ID = 0;

namespace DGL
{
	std::unique_ptr<OpenGLShaderProgram> OpenGLShaderProgram::Create(const Shader& vertexShader, const Shader& fragmentShader)
	{
		const auto glVsShader = CheckNotNull(dynamic_cast<const OpenGLShader*>(&vertexShader), [] { return "Tried to create an OpenGLShaderProgram using a non OpenGL vertex shader"; });
		const auto glFsShader = CheckNotNull(dynamic_cast<const OpenGLShader*>(&fragmentShader), [] { return "Tried to create an OpenGLShaderProgram using a non OpenGL fragment shader"; });

		Check(glVsShader->GetShaderType() == ShaderType::Vertex, [] { return "Provided vertex shader is not of type Vertex"; });
		Check(glFsShader->GetShaderType() == ShaderType::Fragment, [] { return "Provided fragment shader is not of type Fragment"; });

		// Request OpenGL to create the shader program
		const GLuint shaderProgramId = glCreateProgram();
		if (shaderProgramId == INVALID_ID)
		{
			Error("Could not create OpenGL shader program.");
			return nullptr;
		}

		// Attach the shaders and link the program
		glAttachShader(shaderProgramId, glVsShader->GetShaderId());
		glAttachShader(shaderProgramId, glFsShader->GetShaderId());
		glLinkProgram(shaderProgramId);

		// Check for linking errors
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
		if (linkStatus != GL_TRUE)
		{
			// Get the info log length
			GLint infoLogLength;
			glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			// Retrieve the info log
			std::string infoLog(infoLogLength, '\0');
			glGetProgramInfoLog(shaderProgramId, infoLogLength, nullptr, infoLog.data());

			// Clean up the shader program
			glDeleteProgram(shaderProgramId);
			glDetachShader(shaderProgramId, glVsShader->GetShaderId());
			glDetachShader(shaderProgramId, glFsShader->GetShaderId());

			Error("Failed to link shader program: " + infoLog);
			return nullptr;
		}

		// Validate the program
		glValidateProgram(shaderProgramId);

		// Check for validation errors
		GLint validateStatus = GL_FALSE;
		glGetProgramiv(shaderProgramId, GL_VALIDATE_STATUS, &validateStatus);
		if (validateStatus != GL_TRUE)
		{
			// Get the info log length
			GLint infoLogLength;
			glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &infoLogLength);

			// Retrieve the info log
			std::string infoLog(infoLogLength, '\0');
			glGetProgramInfoLog(shaderProgramId, infoLogLength, nullptr, infoLog.data());

			// Clean up the shader program
			glDeleteProgram(shaderProgramId);
			glDetachShader(shaderProgramId, glVsShader->GetShaderId());
			glDetachShader(shaderProgramId, glFsShader->GetShaderId());

			Error("Failed to validate shader program: " + infoLog);
			return nullptr;
		}

		// Detach shaders after successful linking
		glDetachShader(shaderProgramId, glVsShader->GetShaderId());
		glDetachShader(shaderProgramId, glFsShader->GetShaderId());

		// We've successfully created, linked and validated the shader program
		return std::unique_ptr<OpenGLShaderProgram>(new OpenGLShaderProgram(shaderProgramId));
	}

	void OpenGLShaderProgram::UploadInt1(const std::string_view name, const std::int32_t x)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1i(m_ShaderProgramId, location, x);
		}
	}

	void OpenGLShaderProgram::UploadInt2(const std::string_view name, const std::int32_t x, const std::int32_t y)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2i(m_ShaderProgramId, location, x, y);
		}
	}

	void OpenGLShaderProgram::UploadInt3(const std::string_view name, const std::int32_t x, const std::int32_t y, const std::int32_t z)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3i(m_ShaderProgramId, location, x, y, z);
		}
	}

	void OpenGLShaderProgram::UploadInt4(const std::string_view name, const std::int32_t x, const std::int32_t y, const std::int32_t z, const std::int32_t w)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4i(m_ShaderProgramId, location, x, y, z, w);
		}
	}

	void OpenGLShaderProgram::UploadFloat1(const std::string_view name, const float x)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1f(m_ShaderProgramId, location, x);
		}
	}

	void OpenGLShaderProgram::UploadFloat2(const std::string_view name, const float x, const float y)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2f(m_ShaderProgramId, location, x, y);
		}
	}

	void OpenGLShaderProgram::UploadFloat3(const std::string_view name, const float x, const float y, const float z)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3f(m_ShaderProgramId, location, x, y, z);
		}
	}

	void OpenGLShaderProgram::UploadFloat4(const std::string_view name, const float x, const float y, const float z, const float w)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4f(m_ShaderProgramId, location, x, y, z, w);
		}
	}

	void OpenGLShaderProgram::UploadFloat1Array(const std::string_view name, const std::span<const float, 1> data)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform1fv(m_ShaderProgramId, location, static_cast<GLsizei>(data.size()), data.data());
		}
	}

	void OpenGLShaderProgram::UploadFloat2Array(const std::string_view name, const std::span<const float, 2> data)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform2fv(m_ShaderProgramId, location, static_cast<GLsizei>(data.size()), data.data());
		}
	}

	void OpenGLShaderProgram::UploadFloat3Array(const std::string_view name, const std::span<const float, 3> data)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform3fv(m_ShaderProgramId, location, static_cast<GLsizei>(data.size()), data.data());
		}
	}

	void OpenGLShaderProgram::UploadFloat4Array(const std::string_view name, const std::span<const float, 4> data)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniform4fv(m_ShaderProgramId, location, static_cast<GLsizei>(data.size()), data.data());
		}
	}

	void OpenGLShaderProgram::UploadMatrix4x4(const std::string_view name, const std::span<const float, 16> matrix)
	{
		if (const GLint location = GetUniformLocation(name); location != -1)
		{
			glProgramUniformMatrix4fv(m_ShaderProgramId, location, 1, GL_FALSE, matrix.data());
		}
	}

	void OpenGLShaderProgram::Bind()
	{
		glUseProgram(m_ShaderProgramId);
	}

	OpenGLShaderProgram::OpenGLShaderProgram(const GLuint shaderProgramId):
		m_ShaderProgramId(shaderProgramId)
	{
		Require(m_ShaderProgramId != INVALID_ID, [] { return std::format("Could not create shaderprogram with resource-id {}.", INVALID_ID); });
	}

	GLint OpenGLShaderProgram::GetUniformLocation(const std::string_view name)
	{
		return glGetProgramResourceLocation(m_ShaderProgramId, GL_UNIFORM, name.data());
	}

}