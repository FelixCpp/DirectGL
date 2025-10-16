// Project Name : DirectGL-Brushes
// File Name    : Brushes-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>
#include <memory>

export module DirectGL.Brushes:SolidColorBrush;

import :ShaderProgram;

import DirectGL.Renderer;
import DirectGL.Math;

export namespace DGL::Brushes
{
	class SolidColorBrush
	{
	public:

		static std::unique_ptr<SolidColorBrush> Create(Renderer::Color color);

		void SetColor(Renderer::Color color);
		Renderer::Color GetColor() const;

		void UploadUniforms(const Math::Matrix4x4& projectionViewMatrix, const Math::Matrix4x4& modelMatrix);

	private:

		explicit SolidColorBrush(
			std::unique_ptr<ShaderProgram> shaderProgram,
			Renderer::Color color
		);

		Renderer::Color m_Color;
		std::unique_ptr<ShaderProgram> m_ShaderProgram;

	};
}