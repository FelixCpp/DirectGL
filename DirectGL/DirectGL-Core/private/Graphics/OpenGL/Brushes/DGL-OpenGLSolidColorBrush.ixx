// Project Name : DirectGL
// File Name    : DGL-OpenGLSolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/28

module;

#include <memory>

export module DGL:OpenGLSolidColorBrush;

import :SolidColorBrush;
import :OpenGLShaderProgram;

import :Color;

namespace DGL
{
	class OpenGLSolidColorBrush : public SolidColorBrush
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLSolidColorBrush> Create(Color color);

		void SetColor(Color color) override;
		[[nodiscard]] Color GetColor() const override;

		void Apply(const Camera& camera) override;

	private:

		explicit OpenGLSolidColorBrush(std::unique_ptr<OpenGLShaderProgram> shaderProgram, Color color);

		Color m_Color;
		std::unique_ptr<OpenGLShaderProgram> m_ShaderProgram;

	};
}