// Project Name : DirectGL
// File Name    : DGL-OpenGLSolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/28

module;

#include <glad/gl.h>

#include <memory>

export module DGL:OpenGLSolidColorBrush;

import :SolidColorBrush;

import :Color;

namespace DGL
{
	class OpenGLSolidColorBrush : public SolidColorBrush
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLSolidColorBrush> Create(Color color);

		~OpenGLSolidColorBrush() override;

		void SetColor(Color color) override;
		[[nodiscard]] Color GetColor() const override;

		void Apply(const Camera& camera) override;

	private:

		explicit OpenGLSolidColorBrush(
			Color color,
			GLuint shaderProgramId,
			GLint colorUniformLocation,
			GLint viewProjectionUniformLocation
		);

		Color	m_Color;
		GLuint	m_ShaderProgramId;
		GLint	m_ColorUniformLocation;
		GLint	m_ViewProjectionUniformLocation;

	};
}