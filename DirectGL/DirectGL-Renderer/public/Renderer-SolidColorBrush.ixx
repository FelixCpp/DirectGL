// Project Name : DirectGL-Renderer
// File Name    : Renderer-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>
#include <memory>

export module DirectGL.Renderer:SolidColorBrush;

import :Color;
import :ShaderProgram;

import Math;

export namespace DGL::Renderer
{
	class SolidColorBrush
	{
	public:

		static std::unique_ptr<SolidColorBrush> Create(Color color);

		void SetColor(Color color);
		Color GetColor() const;

		void UploadUniforms(const Math::Matrix4x4& projectionViewMatrix);

	private:

		explicit SolidColorBrush(
			std::unique_ptr<ShaderProgram> shaderProgram,
			Color color
		);

		Color m_Color;
		std::unique_ptr<ShaderProgram> m_ShaderProgram;

	};
}