// Project Name : DirectGL
// File Name    : DirectGL-ShapeBuilder.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

module;

#include <vector>
#include <optional>

export module DirectGL:ShapeBuilder;

import DirectGL.Math;

import :ShapeMode;
import :QuadRenderer;
import :Color;

namespace DGL
{
	struct ShapeVertex
	{
		Math::Float3	Position;
		color_t			FillColor;
		color_t			StrokeColor;
		float			StrokeWeight;
	};

	class ShapeBuilder
	{
	public:

		explicit ShapeBuilder(QuadRenderer& quadRenderer);

		void Begin(ShapeMode mode);
		void End();

		void AddVertex(const ShapeVertex& vertex);

	private:

		void UploadVerticesAsPoints();
		void UploadVerticesAsLines();
		void UploadVerticesAsTriangles();
		void UploadVerticesAsTriangleStrip();
		void UploadVerticesAsTriangleFan();
		void UploadVerticesAsQuads() const;
		void UploadVerticesAsQuadStrip() const;

		static QuadRenderer::QuadProperties CreateQuadProperties(const ShapeVertex& v1, const ShapeVertex& v2, const ShapeVertex& v3, const ShapeVertex& v4);

		std::optional<ShapeMode> m_CurrentShapeMode;
		std::vector<ShapeVertex> m_Vertices;
		QuadRenderer* m_QuadRenderer;

	};
}