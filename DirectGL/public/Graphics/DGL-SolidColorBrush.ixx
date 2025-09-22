// Project Name : DirectGL
// File Name    : DGL-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:SolidColorBrush;

import :Brush;
import :ShaderProgram;

export namespace DGL
{
	class SolidColorBrush : public Brush
	{
	public:

		explicit SolidColorBrush(float red, float green, float blue, float alpha = 1.0f);

		void SetColor(float red, float green, float blue, float alpha = 1.0f);
		void Apply() override;

	private:

		ShaderProgram m_Shader;
		float m_Red, m_Green, m_Blue, m_Alpha;

	};
}