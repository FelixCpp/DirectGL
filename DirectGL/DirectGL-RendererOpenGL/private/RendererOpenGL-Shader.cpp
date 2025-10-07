module;

#include <glad/gl.h>

#include <format>
#include <string>

module DirectGL.RendererOpenGL;

import Preconditions;

import :Shader;

namespace DGL::RendererOpenGL
{
	constexpr std::string_view GetShaderTypeName(const ShaderType type)
	{
		switch (type)
		{
			case ShaderType::Vertex: return "ShaderType::Vertex";
			case ShaderType::Fragment: return "ShaderType::Fragment";
			default: System::Error(std::format("Unknown ShaderType: {}", type));
		}
	}

	std::unique_ptr<OpenGLShader> OpenGLShader::Create(const std::string_view source, const ShaderType type)
	{
		const GLenum shaderType = [type]
		{
			switch (type)
			{
				case ShaderType::Vertex: return GL_VERTEX_SHADER;
				case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
				default: System::Error(std::format("Unknown ShaderType: {}", type));
			}
		}();

		const GLuint shaderId = glCreateShader(shaderType);
		if (shaderId == 0)
		{
			s_Logger.Log(LogLevel::Error, std::format("Failed to create shader of type {} on GPU side.", GetShaderTypeName(type)));
			return nullptr;
		}

		const char* sourcePtr = source.data();
		glShaderSource(shaderId, 1, &sourcePtr, nullptr);
		glCompileShader(shaderId);

		GLint compilationSucceeded = GL_FALSE;
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationSucceeded);
		if (compilationSucceeded != GL_TRUE)
		{
			GLint infoLogLength = 0;
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			std::string buffer(static_cast<size_t>(infoLogLength), '\0');
			glGetShaderInfoLog(shaderId, buffer.length(), &infoLogLength, buffer.data());
			glDeleteShader(shaderId);

			s_Logger.Log(LogLevel::Error, std::format("Failed to compile shader of type {}. Info Log: {}", GetShaderTypeName(type), buffer));
			return nullptr;
		}

		return std::unique_ptr<OpenGLShader>(new OpenGLShader(shaderId));
	}

	GLuint OpenGLShader::GetRendererId() const
	{
		return m_RendererId;
	}

	OpenGLShader::OpenGLShader(const GLuint shaderId):
		m_RendererId(shaderId)
	{
	}
}