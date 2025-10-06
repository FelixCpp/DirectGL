module;

#include <glad/gl.h>
#include <format>
#include <stdexcept>

module DGL;

import Preconditions;

namespace DGL
{
	std::unique_ptr<OpenGLShader> OpenGLShader::Create(const std::string_view source, const ShaderType type)
	{
		// Map ShaderType to OpenGL shader type
		const GLenum glShaderType = [&type]
		{
			switch (type)
			{
				case ShaderType::Vertex:   return GL_VERTEX_SHADER;
				case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
				case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
				case ShaderType::Compute:  return GL_COMPUTE_SHADER;
				default: throw std::invalid_argument(std::format("Unsupported shader type: {}.", static_cast<int>(type)));
			}
		}();

		// Request OpenGL to create the shader
		const GLuint shaderId = glCreateShader(glShaderType);
		if (shaderId == 0)
		{
			Error("Could not create OpenGL shader.");
			return nullptr;
		}

		// Attach the source code and compile the shader
		const GLchar* sourceCStr = source.data();
		glShaderSource(shaderId, 1, &sourceCStr, nullptr);
		glCompileShader(shaderId);

		// Check for compilation errors
		// Note that the error is being collected by the OpenGL-Debugging mechanism as well
		GLint compileStatus;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);
		if (compileStatus != GL_TRUE)
		{
			// Get the info log length
			GLint logLength;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);

			// Retrieve the info log
			std::string infoLog(static_cast<size_t>(logLength), '\0');
			glGetShaderInfoLog(shaderId, logLength, nullptr, infoLog.data());

			// Clean up the shader object
			glDeleteShader(shaderId);

			// Print out the error
			Error(std::format("Failed to compile OpenGL shader (ID: {}). Info log: {}", shaderId, infoLog));
			return nullptr;
		}

		// We've successfully created and compiled the shader
		return std::unique_ptr<OpenGLShader>(new OpenGLShader(shaderId, type));
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteShader(m_ShaderId);
	}

	ShaderType OpenGLShader::GetShaderType() const
	{
		return m_Type;
	}

	GLuint OpenGLShader::GetShaderId() const
	{
		return m_ShaderId;
	}

	OpenGLShader::OpenGLShader(const GLuint shaderId, const ShaderType type) :
		m_ShaderId(shaderId),
		m_Type(type)
	{
		System::Require(shaderId != 0, [] { return std::format("Cannot create opengl shader using an invalid resource-id of {}.", 0); });
	}
}