// Project Name : DirectGL-RendererOpenGL
// File Name    : RendererOpenGL-Shader.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>

#include <memory>
#include <string_view>

export module DirectGL.RendererOpenGL:Shader;

namespace DGL::RendererOpenGL
{
	enum class ShaderType
	{
		Vertex,
		Fragment,
	};

	class OpenGLShader
	{
	public:

		static std::unique_ptr<OpenGLShader> Create(std::string_view source, ShaderType type);

		GLuint GetRendererId() const;

	private:

		explicit OpenGLShader(GLuint shaderId);

		GLuint m_RendererId;

	};
}