// Project Name : DirectGL
// File Name    : DirectGL-ShapeBuilder.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

module;

#include <vector>
#include <optional>

export module DirectGL:ShapeBuilder;

import DirectGL.Math;

import :MeshBuilder;
import :ShapeMode;
import :Color;
import :DepthProvider;

namespace DGL
{
	struct ShapeVertex
	{
		Math::Float2 Position;
		color_t FillColor;
		color_t StrokeColor;
	};

	struct ShapeBuildingProperties
	{
		float StrokeWeight;
		StrokeJoin JoinStyle;
		StrokeCap StrokeCap;
		bool IsStrokeEnabled;
		bool IsFillEnabled;
		bool ShouldCloseStroke;
	};

	typedef std::vector<Mesh> Shape;

	class ShapeBuilder
	{
	public:

		explicit ShapeBuilder(DepthProvider& depthProvider);

		void Begin(ShapeMode mode);
		Shape End(const ShapeBuildingProperties& properties);

		void AddVertex(const ShapeVertex& vertex);

	private:

		Shape UploadVerticesAsPoints(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsLines(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsTriangles(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsTriangleStrip(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsTriangleFan(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsQuads(const ShapeBuildingProperties& properties);
		Shape UploadVerticesAsQuadStrip(const ShapeBuildingProperties& properties);

		std::optional<ShapeMode> m_CurrentShapeMode;
		std::vector<ShapeVertex> m_Vertices;

		DepthProvider* m_DepthProvider;

	};
}