// Project Name : DirectGL
// File Name    : DirectGL-VertexRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <glad/gl.h>

#include <span>
#include <cstdint>
#include <memory>

export module DirectGL:VertexRenderer;

import DirectGL.Math;

namespace DGL
{
	class VertexRenderer
	{
	public:

		VertexRenderer();

		void BeginDraw(const Math::Matrix4x4& projectionViewMatrix);
		void EndDraw();

		void Submit(
			const std::span<const Math::Float3>& positions,
			const std::span<const Math::Float4>& colors,
			const std::span<const uint32_t>& indices,
			const Math::Matrix4x4& modelMatrix
		);

	private:

		void Flush();

		GLuint m_VertexArray;
		GLuint m_PositionBuffer;
		GLuint m_ColorBuffer;
		GLuint m_ElementBuffer;

		GLuint m_ShaderProgram;

		GLint m_ProjectionViewMatrixLocation;
		GLint m_ModelMatrixLocation;

		size_t m_DrawCalls;

	};
}