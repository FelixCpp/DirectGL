module;

#include <optional>
#include <array>

module DirectGL;

import Preconditions;

import :ShapeBuilder;

namespace DGL
{
	MeshVertex ShapeVertex::ToFilledMeshVertex() const
	{
		return MeshVertex {
			.Position = Position,
			.Color = FillColor
		};
	}

	MeshVertex ShapeVertex::ToStrokedMeshVertex() const
	{
		return MeshVertex {
			.Position = Position,
			.Color = StrokeColor
		};
	}

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
			Mesh pointMesh = m_MeshBuilder.GeneratePointMesh(
				m_Vertices[i].ToFilledMeshVertex(),
				PointMeshProperties {
					.Radius = properties.StrokeWeight,
					.Segments = 12, // TODO(Felix): Make configurable
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(pointMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsLines(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 1; i < m_Vertices.size(); i += 2)
		{
			Mesh lineMesh = m_MeshBuilder.GenerateLineMesh(
				std::array {
					m_Vertices[i - 1].ToFilledMeshVertex(),
					m_Vertices[i - 0].ToFilledMeshVertex()
				},
				LineMeshProperties {
					.StartCap = properties.StartCap,
					.EndCap = properties.EndCap,
					.StrokeWeight = properties.StrokeWeight,
					.RoundedCapSegments = 12, // TODO(Felix): Make configurable
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(lineMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangles(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); i += 3)
		{
			Mesh triangleMesh = m_MeshBuilder.GenerateTriangleMesh(
				std::array {
					m_Vertices[i - 2].ToFilledMeshVertex(),
					m_Vertices[i - 1].ToFilledMeshVertex(),
					m_Vertices[i - 0].ToFilledMeshVertex(),
				},
				TriangleMeshProperties {
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			Mesh outlinedMesh = m_MeshBuilder.GenerateOutlinedMesh(
				std::array{
					m_Vertices[i - 2].ToStrokedMeshVertex(),
					m_Vertices[i - 1].ToStrokedMeshVertex(),
					m_Vertices[i - 0].ToStrokedMeshVertex(),
				},
				OutlinedMeshProperties {
					.StrokeWeight = properties.StrokeWeight,
					.JoinStyle = properties.JoinStyle,
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(triangleMesh);
			shape.StrokeShapes.emplace_back(outlinedMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangleStrip(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			Mesh triangleMesh = m_MeshBuilder.GenerateTriangleMesh(
				std::array {
					m_Vertices[i - 2].ToFilledMeshVertex(),
					m_Vertices[i - 1].ToFilledMeshVertex(),
					m_Vertices[i - 0].ToFilledMeshVertex()
				},
				TriangleMeshProperties {
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			Mesh outlinedMesh = m_MeshBuilder.GenerateOutlinedMesh(
				std::array{
					m_Vertices[i - 2].ToStrokedMeshVertex(),
					m_Vertices[i - 1].ToStrokedMeshVertex(),
					m_Vertices[i - 0].ToStrokedMeshVertex()
				},
				OutlinedMeshProperties {
					.StrokeWeight = properties.StrokeWeight,
					.JoinStyle = properties.JoinStyle,
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(triangleMesh);
			shape.StrokeShapes.emplace_back(outlinedMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsTriangleFan(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 2; i < m_Vertices.size(); ++i)
		{
			Mesh triangleMesh = m_MeshBuilder.GenerateTriangleMesh(
				std::array {
					m_Vertices[0].ToFilledMeshVertex(),
					m_Vertices[i - 1].ToFilledMeshVertex(),
					m_Vertices[i - 0].ToFilledMeshVertex()
				},
				TriangleMeshProperties {
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			Mesh outlinedMesh = m_MeshBuilder.GenerateOutlinedMesh(
				std::array{
					m_Vertices[0].ToStrokedMeshVertex(),
					m_Vertices[i - 1].ToStrokedMeshVertex(),
					m_Vertices[i - 0].ToStrokedMeshVertex()
				},
				OutlinedMeshProperties {
					.StrokeWeight = properties.StrokeWeight,
					.JoinStyle = properties.JoinStyle,
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(triangleMesh);
			shape.StrokeShapes.emplace_back(outlinedMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsQuads(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 4; i < m_Vertices.size(); i += 4)
		{
			Mesh quadMesh = m_MeshBuilder.GenerateQuadMesh(
				std::array {
					m_Vertices[i - 4].ToFilledMeshVertex(),
					m_Vertices[i - 3].ToFilledMeshVertex(),
					m_Vertices[i - 2].ToFilledMeshVertex(),
					m_Vertices[i - 1].ToFilledMeshVertex()
				},
				QuadMeshProperties {
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			Mesh outlinedMesh = m_MeshBuilder.GenerateOutlinedMesh(
				std::array {
					m_Vertices[i - 4].ToStrokedMeshVertex(),
					m_Vertices[i - 3].ToStrokedMeshVertex(),
					m_Vertices[i - 2].ToStrokedMeshVertex(),
					m_Vertices[i - 1].ToStrokedMeshVertex()
				},
				OutlinedMeshProperties {
					.StrokeWeight = properties.StrokeWeight,
					.JoinStyle = properties.JoinStyle,
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(quadMesh);
			shape.StrokeShapes.emplace_back(outlinedMesh);
		}

		return shape;
	}

	Shape ShapeBuilder::UploadVerticesAsQuadStrip(const ShapeBuildingProperties& properties)
	{
		Shape shape;

		for (size_t i = 3; i < m_Vertices.size(); i += 2)
		{
			Mesh quadMesh = m_MeshBuilder.GenerateQuadMesh(
				std::array {
					m_Vertices[i - 3].ToFilledMeshVertex(),
					m_Vertices[i - 2].ToFilledMeshVertex(),
					m_Vertices[i - 1].ToFilledMeshVertex(),
					m_Vertices[i - 0].ToFilledMeshVertex()
				},
				QuadMeshProperties {
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			Mesh outlinedMesh = m_MeshBuilder.GenerateOutlinedMesh(
				std::array{
					m_Vertices[i - 3].ToStrokedMeshVertex(),
					m_Vertices[i - 2].ToStrokedMeshVertex(),
					m_Vertices[i - 1].ToStrokedMeshVertex(),
					m_Vertices[i - 0].ToStrokedMeshVertex()
				},
				OutlinedMeshProperties {
					.StrokeWeight = properties.StrokeWeight,
					.JoinStyle = properties.JoinStyle,
					.Depth = m_DepthProvider->GetAndIncrement(),
				}
			);

			shape.FillShapes.emplace_back(quadMesh);
			shape.StrokeShapes.emplace_back(outlinedMesh);
		}

		return shape;
	}
}