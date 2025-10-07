// Project Name : DirectGL-Renderer
// File Name    : Renderer-SolidColorBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <glad/gl.h>
#include <memory>

export module DirectGL.Renderer:SolidColorBrush;

import :Color;

import Math;

export namespace DGL::Renderer
{
	class SolidColorBrush
	{
	public:

		static std::unique_ptr<SolidColorBrush> Create(Color color);

		~SolidColorBrush();

		void SetColor(Color color);
		Color GetColor() const;

		void UploadUniforms(const Math::Matrix4x4& projectionViewMatrix);

	private:

		explicit SolidColorBrush(
			GLuint shaderProgramId,
			GLint colorLocation,
			GLint projectionViewMatrixLocation,
			Color color
		);

		Color m_Color;
		GLuint m_ShaderProgramId;
		GLint m_ColorLocation;
		GLint m_ProjectionViewMatrixLocation;

	};
}