// Project Name : DirectGL
// File Name    : DGL-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <memory>

export module DGL:SolidColorBrush;

import :Brush;
import :Color;
import :ShaderProgram;

export namespace DGL
{
	class SolidColorBrush : public Brush
	{
	public:

		static std::unique_ptr<SolidColorBrush> Create(Color color);

		void SetColor(Color color);
		Color GetColor() const;
		void Apply() override;

	private:

		explicit SolidColorBrush(std::unique_ptr<ShaderProgram> shaderProgram, Color color);

		std::unique_ptr<ShaderProgram> m_Shader;
		Color m_Color;

	};
}