// Project Name : DirectGL
// File Name    : DGL-OpenGLShader.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <glad/gl.h>

#include <memory>
#include <string_view>

export module DGL:OpenGLShader;

import :Shader;

namespace DGL
{
	class OpenGLShader : public Shader
	{
	public:

		static std::unique_ptr<OpenGLShader> Create(std::string_view source, ShaderType type);

		~OpenGLShader() override;

		[[nodiscard]] GLuint GetShaderId() const;
		[[nodiscard]] ShaderType GetShaderType() const override;

	private:

		explicit OpenGLShader(GLuint shaderId, ShaderType type);

		GLuint m_ShaderId;
		ShaderType m_Type;

	};
}