module;

#include <optional>
#include <array>

module DirectGL;

import Preconditions;

import :ShapeBuilder;

namespace DGL
{
	ShapeBuilder::ShapeBuilder(DepthProvider& depthProvider):
		m_CurrentShapeMode(std::nullopt),
		m_DepthProvider(&depthProvider)
	{
	}

	void ShapeBuilder::Begin(const ShapeMode mode)
	{
		m_CurrentShapeMode = mode;
	}

	Shape ShapeBuilder::End(const ShapeBuildingProperties& properties)
	{
		// Check if there are vertices to process
		if (m_Vertices.empty())
		{
			return Shape{};
		}

		Shape result = [this, &properties] {
			switch (m_CurrentShapeMode.value())
			{
				case ShapeMode::Points: return UploadVerticesAsPoints(properties);
				case ShapeMode::Lines: return UploadVerticesAsLines(properties);
				case ShapeMode::Triangles: return UploadVerticesAsTriangles(properties);
				case ShapeMode::TriangleStrip: return UploadVerticesAsTriangleStrip(properties);
				case ShapeMode::TriangleFan: return UploadVerticesAsTriangleFan(properties);
				case ShapeMode::Quads: return UploadVerticesAsQuads(properties);
				case ShapeMode::QuadStrip: return UploadVerticesAsQuadStrip(properties);
				default: System::Error("Unknown shape mode in ShapeBuilder::End()");
			}
		}();

		m_CurrentShapeMode.reset();
		m_Vertices.clear();

		return result;
	}

	void ShapeBuilder::AddVertex(const ShapeVertex& vertex)
	{
		if (not m_CurrentShapeMode.has_value())
		{
			// TODO(Felix): Print out a warning that no shape has been started
			return;
		}

		m_Vertices.emplace_back(vertex);
	}

	Shape ShapeBuilder::UploadVerticesAsPoints(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 0; i < m_Vertices.size(); ++i)
		{
			if (properties.IsStrokeEnabled)
			{
				Mesh pointMesh = MeshBuilder::GenerateEllipseMesh(
					m_Vertices[i].Position,
					m_Vertices[i].StrokeColor,
					Math::Radius::Circular(properties.StrokeWeight),
					32, // TODO(Felix): Make configurable
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(pointMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsLines(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 1; i < m_Vertices.size(); i += 2)
		{
			if (properties.IsStrokeEnabled)
			{
				Mesh lineMesh = MeshBuilder::GenerateLineMesh(
					std::array{ m_Vertices[i - 1].Position, m_Vertices[i - 0].Position },
					std::array{ m_Vertices[i - 1].StrokeColor, m_Vertices[i - 0].StrokeColor },
					properties.StrokeCap,
					properties.StrokeWeight,
					12, // TODO(Felix): Make configurable
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(lineMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangles(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); i += 3)
		{
			const ShapeVertex& v0 = m_Vertices[i - 2];
			const ShapeVertex& v1 = m_Vertices[i - 1];
			const ShapeVertex& v2 = m_Vertices[i - 0];

			if (properties.IsFillEnabled)
			{
				Mesh triangleMesh = MeshBuilder::GenerateTriangleMesh(
					std::array{ v0.Position, v1.Position, v2.Position, },
					std::array{ v0.FillColor, v1.FillColor, v2.FillColor, },
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(triangleMesh);
			}

			if (properties.IsStrokeEnabled)
			{
				Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
					std::array{ v0.Position, v1.Position, v2.Position, },
					std::array{ v0.StrokeColor, v1.StrokeColor, v2.StrokeColor, },
					properties.StrokeWeight,
					properties.JoinStyle,
					properties.StrokeCap,
					properties.ShouldCloseStroke,
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(outlinedMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangleStrip(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			const ShapeVertex& v0 = m_Vertices[i - 2];
			const ShapeVertex& v1 = m_Vertices[i - 1];
			const ShapeVertex& v2 = m_Vertices[i - 0];

			if (properties.IsFillEnabled)
			{
				Mesh triangleMesh = MeshBuilder::GenerateTriangleMesh(
					std::array{ v0.Position, v1.Position, v2.Position },
					std::array{ v0.FillColor, v1.FillColor, v2.FillColor },
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(triangleMesh);
			}

			if (properties.IsStrokeEnabled)
			{
				Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
					std::array{ v0.Position, v1.Position, v2.Position },
					std::array{ v0.StrokeColor, v1.StrokeColor, v2.StrokeColor },
					properties.StrokeWeight,
					properties.JoinStyle,
					properties.StrokeCap,
					properties.ShouldCloseStroke,
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(outlinedMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangleFan(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			const ShapeVertex& v0 = m_Vertices[0];
			const ShapeVertex& v1 = m_Vertices[i - 1];
			const ShapeVertex& v2 = m_Vertices[i - 0];

			if (properties.IsFillEnabled)
			{
				Mesh triangleMesh = MeshBuilder::GenerateTriangleMesh(
					std::array{ v0.Position, v1.Position, v2.Position },
					std::array{ v0.FillColor, v1.FillColor, v2.FillColor },
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(triangleMesh);
			}

			if (properties.IsStrokeEnabled)
			{
				Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
					std::array{ v0.Position, v1.Position, v2.Position },
					std::array{ v0.StrokeColor, v1.StrokeColor, v2.StrokeColor },
					properties.StrokeWeight,
					properties.JoinStyle,
					properties.StrokeCap,
					properties.ShouldCloseStroke,
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(outlinedMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsQuads(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 3; i < m_Vertices.size(); i += 4)
		{
			const ShapeVertex& v0 = m_Vertices[i - 3];
			const ShapeVertex& v1 = m_Vertices[i - 2];
			const ShapeVertex& v2 = m_Vertices[i - 1];
			const ShapeVertex& v3 = m_Vertices[i - 0];

			if (properties.IsFillEnabled)
			{
				Mesh triangleMesh = MeshBuilder::GenerateQuadMesh(
					std::array{ v0.Position, v1.Position, v2.Position, v3.Position },
					std::array{ v0.FillColor, v1.FillColor, v2.FillColor, v3.FillColor },
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(triangleMesh);
			}

			if (properties.IsStrokeEnabled)
			{
				Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
					std::array{ v0.Position, v1.Position, v2.Position, v3.Position },
					std::array{ v0.StrokeColor, v1.StrokeColor, v2.StrokeColor, v3.StrokeColor },
					properties.StrokeWeight,
					properties.JoinStyle,
					properties.StrokeCap,
					properties.ShouldCloseStroke,
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(outlinedMesh);
			}
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsQuadStrip(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		// Index = 4
		// First iteration = Vertices 0,1,2,3
		// Index += 2
		// Second iteration = Vertices 2,3,4,5

		for (size_t i = 3; i < m_Vertices.size(); i += 2)
		{
			const ShapeVertex& v0 = m_Vertices[i - 3];
			const ShapeVertex& v1 = m_Vertices[i - 2];
			const ShapeVertex& v2 = m_Vertices[i - 1];
			const ShapeVertex& v3 = m_Vertices[i - 0];

			if (properties.IsFillEnabled)
			{
				Mesh triangleMesh = MeshBuilder::GenerateQuadMesh(
					std::array{ v0.Position, v1.Position, v2.Position, v3.Position },
					std::array{ v0.FillColor, v1.FillColor, v2.FillColor, v3.FillColor },
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(triangleMesh);
			}

			if (properties.IsStrokeEnabled)
			{
				Mesh outlinedMesh = MeshBuilder::GenerateOutlinedMesh(
					std::array{ v0.Position, v1.Position, v2.Position, v3.Position },
					std::array{ v0.StrokeColor, v1.StrokeColor, v2.StrokeColor, v3.StrokeColor },
					properties.StrokeWeight,
					properties.JoinStyle,
					properties.StrokeCap,
					properties.ShouldCloseStroke,
					m_DepthProvider->GetAndIncrement()
				);

				shape.emplace_back(outlinedMesh);
			}
		}

		return shape;
	}
}