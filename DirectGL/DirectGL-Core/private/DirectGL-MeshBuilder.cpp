module;

#include <cmath>
#include <cstdio>
#include <algorithm>

module DirectGL;

import Preconditions;

import :MeshBuilder;

namespace DGL
{
	Mesh MeshBuilder::GenerateEllipseMesh(const Math::Float2& center, const Math::Float4& color, const Math::Radius radius, const uint32_t segments, float depth)
	{
		if (segments < 3)
		{
			return {};
		}

		std::vector<Math::Float3> positions(segments + 1); // + 1 for the center vertex
		std::vector<Math::Float4> colors(segments + 1); // + 1 for the center vertex
		std::vector<uint32_t> indices(segments * 3); // Each segment forms a triangle with the center, therefore 3 indices per segment

		// The first point is the center
		positions[0] = { center.X, center.Y, depth };
		colors[0] = color;

		// Compute the angle we need to step by for each segment
		const float angleStep = (2.0f * Math::PI) / static_cast<float>(segments);

		// Iterate through each segment to compute the circle points
		for (size_t i = 0; i < segments; ++i)
		{
			// Compute the current angle using the current iteration and the increment per segment
			const float angle = angleStep * static_cast<float>(i);

			// Compute the coordinate of the point on the circle's circumference
			const float x = center.X + std::cos(angle) * radius.X;
			const float y = center.Y + std::sin(angle) * radius.Y;

			const size_t vertexIndex = i + 1; // +1 to account for the center vertex at index 0
			positions[vertexIndex] = { x, y, depth };
			colors[vertexIndex] = color;

			// Compute the indices for the triangle fan
			indices[i * 3 + 0] = 0; // Center vertex
			indices[i * 3 + 1] = static_cast<uint32_t>(vertexIndex);
			indices[i * 3 + 2] = static_cast<uint32_t>(vertexIndex % segments + 1); // Wrap around to the first circumference vertex
		}

		return Mesh {
			.Positions = std::move(positions),
			.Colors = std::move(colors),
			.Indices = std::move(indices)
		};
	}

	void InsertButtLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Float2& offset, const float depth)
	{
		// Add the positions
		positions.emplace_back(point.X + offset.X, point.Y + offset.Y, depth);
		positions.emplace_back(point.X - offset.X, point.Y - offset.Y, depth);

		// Add the colors
		colors.emplace_back(color);
		colors.emplace_back(color);
	}

	void InsertSquareLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Float2& direction, const Math::Float2& offset, const float depth)
	{
		const Math::Float2 capOffset = direction * offset.Length();

		// Add the positions
		positions.emplace_back(point.X + offset.X + capOffset.X, point.Y + offset.Y + capOffset.Y, depth);
		positions.emplace_back(point.X - offset.X + capOffset.X, point.Y - offset.Y + capOffset.Y, depth);

		// Add the colors
		colors.emplace_back(color);
		colors.emplace_back(color);
	}

	void InsertRoundLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Angle startAngle, const Math::Angle sweepAngle, const float radius, uint32_t segments, const float depth)
	{
		const float startRadians = startAngle.AsRadians();
		const float sweepRadians = sweepAngle.AsRadians();

		// Compute the angle step size per segment
		const float angleStepSize = sweepRadians / static_cast<float>(segments);

		for (uint32_t i = 0; i <= segments; ++i)
		{
			const float angle = startRadians + angleStepSize * static_cast<float>(i);
			const float x = point.X + std::cos(angle) * radius;
			const float y = point.Y + std::sin(angle) * radius;

			positions.emplace_back(x, y, depth);
			colors.emplace_back(color);
		}
	}

	Mesh MeshBuilder::GenerateLineMesh(const std::span<const Math::Float2, 2>& points, const std::span<const Math::Float4, 2>& colors, StrokeCap strokeCap, float strokeWeight, uint32_t roundedCapSegments, float depth)
	{
		// Make some optimizations for degenerate lines
		if (strokeWeight < 3.0f)
		{
			strokeCap = StrokeCap::Butt;
		}

		// Compute the direction vector of the line
		const float halfStrokeWeight = strokeWeight * 0.5f;
		const Math::Float2 direction = (points[1] - points[0]).Normalized();
		const Math::Float2 offset = direction.Perpendicular() * halfStrokeWeight;
		const Math::Angle startAngle = direction.Heading() + Math::Degrees(90.0f);

		std::vector<Math::Float3> positions;
		std::vector<Math::Float4> meshColors;

		// Insert the start cap
		switch (strokeCap.Start)
		{
			case StrokeCapType::Butt: InsertButtLineCap(positions, meshColors, points[0], colors[0], offset, depth); break;
			case StrokeCapType::Square: InsertSquareLineCap(positions, meshColors, points[0], colors[0], -direction, offset, depth); break;
			case StrokeCapType::Round: InsertRoundLineCap(positions, meshColors, points[0], colors[0], startAngle, Math::Degrees(180.0f), halfStrokeWeight, roundedCapSegments, depth); break;
			default: System::Error("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
		}

		// Store the number of positions used for the start cap
		const size_t startPositionCount = positions.size();

		// Insert the end cap
		switch (strokeCap.End)
		{
			case StrokeCapType::Butt: InsertButtLineCap(positions, meshColors, points[1], colors[1], offset, depth); break;
			case StrokeCapType::Square: InsertSquareLineCap(positions, meshColors, points[1], colors[1], direction, offset, depth); break;
			case StrokeCapType::Round: InsertRoundLineCap(positions, meshColors, points[1], colors[1], startAngle, Math::Degrees(-180.0f), halfStrokeWeight, roundedCapSegments, depth); break;
			default: System::Error("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
		}

		// Store the number of positions used for the end cap
		const size_t endPositionCount = positions.size() - startPositionCount;

		// Now we need to create the indices for the line mesh.
		std::vector<uint32_t> indices;

		// The indices are going to be connected like a triangle strip between the start and end caps.
		for (size_t i = 0; i < std::max(startPositionCount, endPositionCount); ++i)
		{
			const size_t currentStartIndex = std::min(i, startPositionCount - 1);
			const size_t nextStartIndex = std::min(i + 1, startPositionCount - 1);
			const size_t currentEndIndex = startPositionCount + std::min(i, endPositionCount - 1);
			const size_t nextEndIndex = startPositionCount + std::min(i + 1, endPositionCount - 1);

			// First triangle
			indices.emplace_back(static_cast<uint32_t>(currentStartIndex));
			indices.emplace_back(static_cast<uint32_t>(currentEndIndex));
			indices.emplace_back(static_cast<uint32_t>(nextStartIndex));

			// Second triangle
			indices.emplace_back(static_cast<uint32_t>(nextStartIndex));
			indices.emplace_back(static_cast<uint32_t>(currentEndIndex));
			indices.emplace_back(static_cast<uint32_t>(nextEndIndex));
		}

		return Mesh {
			.Positions = std::move(positions),
			.Colors = std::move(meshColors),
			.Indices = std::move(indices)
		};
	}

	Mesh MeshBuilder::GenerateTriangleMesh(const std::span<const Math::Float2, 3>& points, const std::span<const Math::Float4, 3>& colors, float depth)
	{
		return Mesh {
			.Positions = {
				{ points[0].X, points[0].Y, depth },
				{ points[1].X, points[1].Y, depth },
				{ points[2].X, points[2].Y, depth }
			},
			.Colors = {
				colors[0],
				colors[1],
				colors[2]
			},
			.Indices = { 0, 1, 2 }
		};
	}

	Mesh MeshBuilder::GenerateQuadMesh(const std::span<const Math::Float2, 4>& points, const std::span<const Math::Float4, 4>& colors, float depth)
	{
		return Mesh {
			.Positions = {
				{ points[0].X, points[0].Y, depth },
				{ points[1].X, points[1].Y, depth },
				{ points[2].X, points[2].Y, depth },
				{ points[3].X, points[3].Y, depth }
			},
			.Colors = {
				colors[0],
				colors[1],
				colors[2],
				colors[3]
			},
			.Indices = { 0, 1, 2, 2, 3, 0 }
		};
	}

	Mesh MeshBuilder::GenerateOutlinedMesh(const std::span<const Math::Float2>& points, const std::span<const Math::Float4>& colors, float strokeWeight, StrokeJoin joinStyle, StrokeCap strokeCap, bool closed, float depth)
	{
		return {};
	}
}
