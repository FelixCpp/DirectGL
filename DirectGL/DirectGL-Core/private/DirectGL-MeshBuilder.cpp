module;

#include <cmath>

module DirectGL;

import :MeshBuilder;

namespace DGL
{
	Mesh MeshBuilder::GeneratePointMesh(const MeshVertex& vertex, const PointMeshProperties& properties)
	{
		// Early out if there are not enough segments
		if (properties.Segments < 3)
		{
			return Mesh();
		}

		std::vector<MeshVertex> vertices; // +1 for center vertex
		std::vector<uint32_t> indices;

		const float angleStepSize = (2.0f * Math::PI) / static_cast<float>(properties.Segments);

		vertices.emplace_back(vertex); // Center vertex

		for (uint32_t i = 0; i < properties.Segments; ++i)
		{
			const float angle = angleStepSize * static_cast<float>(i);
			const float x = vertex.Position.X + std::cos(angle) * properties.Radius;
			const float y = vertex.Position.Y + std::sin(angle) * properties.Radius;

			vertices.emplace_back(MeshVertex{
				.Position = Math::Float2 { x, y },
				.Color = vertex.Color
			});

			// Create triangle indices
			indices.emplace_back(0); // Center vertex
			indices.emplace_back(i + 1); // Current outer vertex (+1 because of center vertex)
			indices.emplace_back((i + 1) % properties.Segments + 1); // Next outer vertex (+1 because of center vertex)
		}

		return Mesh {
			.Vertices = std::move(vertices),
			.Indices = std::move(indices),
			.Depth = properties.Depth,
		};
	}

	Mesh MeshBuilder::GenerateLineMesh(const std::span<const MeshVertex, 2>& vertices, const LineMeshProperties& properties)
	{
		return Mesh();
	}

	Mesh MeshBuilder::GenerateTriangleMesh(const std::span<const MeshVertex, 3>& vertices, const TriangleMeshProperties& properties)
	{
		return Mesh {
			.Vertices = { vertices.begin(), vertices.end() },
			.Indices = { 0, 1, 2 },
			.Depth = properties.Depth,
		};
	}

	Mesh MeshBuilder::GenerateQuadMesh(const std::span<const MeshVertex, 4>& vertices, const QuadMeshProperties& properties)
	{
		return Mesh();
	}

	Mesh MeshBuilder::GenerateOutlinedMesh(const std::span<const MeshVertex>& vertices, const OutlinedMeshProperties& properties)
	{
		return Mesh();
	}
}