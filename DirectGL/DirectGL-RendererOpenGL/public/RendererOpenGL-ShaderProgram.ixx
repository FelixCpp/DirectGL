// Project Name : DirectGL-RendererOpenGL
// File Name    : RendererOpenGL-ShaderProgram.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>

#include <memory>
#include <string_view>

export module DirectGL.RendererOpenGL:ShaderProgram;

import DirectGL.Renderer;

export namespace DGL::RendererOpenGL
{
	class OpenGLShaderProgram : public Renderer::ShaderProgram
	{
	public:

		static std::unique_ptr<OpenGLShaderProgram> Create(std::string_view vertexSource, std::string_view fragmentSource);

		GLuint GetRendererId() const;

	private:

		explicit OpenGLShaderProgram(GLuint rendererId);

		GLuint m_RendererId;

	};
}