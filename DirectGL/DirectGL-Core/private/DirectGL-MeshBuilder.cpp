module;

#include <cmath>
#include <cstdio>
#include <algorithm>

module DirectGL;

import DirectGL.ControlFlow;

import :MeshBuilder;

namespace DGL
{
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

	void InsertRoundLineCap(std::vector<Math::Float3>& positions, std::vector<Math::Float4>& colors, const Math::Float2& point, const Math::Float4& color, const Math::Angle startAngle, const Math::Angle sweepAngle, const float radius, const uint32_t segments, const float depth)
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

	Mesh MeshBuilder::GenerateLineMesh(const std::span<const Math::Float2, 2>& points, const std::span<const Math::Float4, 2>& colors, StrokeCap strokeCap, const float strokeWeight, const uint32_t roundedCapSegments, const float depth)
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
			default: ThrowError("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
		}

		// Store the number of positions used for the start cap
		const size_t startPositionCount = positions.size();

		// Insert the end cap
		switch (strokeCap.End)
		{
			case StrokeCapType::Butt: InsertButtLineCap(positions, meshColors, points[1], colors[1], offset, depth); break;
			case StrokeCapType::Square: InsertSquareLineCap(positions, meshColors, points[1], colors[1], direction, offset, depth); break;
			case StrokeCapType::Round: InsertRoundLineCap(positions, meshColors, points[1], colors[1], startAngle, Math::Degrees(-180.0f), halfStrokeWeight, roundedCapSegments, depth); break;
			default: ThrowError("Unknown StrokeCap in MeshBuilder::GenerateLineMesh()");
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

	std::vector<Math::Float2> MeshBuilder::GenerateEllipsePoints(const Math::Float2 center, const Math::Radius radius, const size_t segments)
	{
		std::vector<Math::Float2> points;
		points.reserve(segments);
		const float angleStepSize = Math::Degrees(360.0f).AsRadians() / static_cast<float>(segments);
		for (size_t i = 0; i < segments; ++i)
		{
			const float angle = angleStepSize * static_cast<float>(i);
			const float x = center.X + std::cos(angle) * radius.X;
			const float y = center.Y + std::sin(angle) * radius.Y;
			points.emplace_back(x, y);
		}

		return points;
	}

	Mesh MeshBuilder::GenerateFilledEllipseMesh(const Math::Float2 centerPoint, const std::span<const Math::Float2>& points, const std::span<const Math::Float4>& colors, const float depth)
	{
		std::vector<Math::Float3> meshPositions;
		std::vector<Math::Float4> meshColors;
		std::vector<uint32_t> meshIndices;

		// Center point
		meshPositions.emplace_back(centerPoint, depth);
		meshColors.emplace_back(colors[0]);

		// Perimeter points
		for (size_t i = 0; i < points.size(); ++i)
		{
			meshPositions.emplace_back(points[i], depth);
			meshColors.emplace_back(colors[i]);
		}

		// Create indices for triangle fan
		const uint32_t perimeterCount = static_cast<uint32_t>(points.size());

		for (uint32_t i = 0; i < perimeterCount; ++i)
		{
			const uint32_t currentIndex = i + 1;
			const uint32_t nextIndex = (i + 1) % perimeterCount + 1;
			meshIndices.emplace_back(0);			//!< Center point index
			meshIndices.emplace_back(currentIndex);	//!< Current perimeter point index
			meshIndices.emplace_back(nextIndex);	//!< Next perimeter point index
		}

		return Mesh{
			.Positions = std::move(meshPositions),
			.Colors = std::move(meshColors),
			.Indices = std::move(meshIndices)
		};
	}

	std::array<Math::Float2, 4> MeshBuilder::GenerateQuadPoints(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, const Math::Float2& d)
	{
		return { a, b, c, d };
	}

	Mesh MeshBuilder::GenerateFilledQuadMesh(const std::span<const Math::Float2, 4>& points, const std::span<const Math::Float4, 4>& colors, const float depth)
	{
		return Mesh{
			.Positions = std::vector {
				Math::Float3{ points[0], depth },
				Math::Float3{ points[1], depth },
				Math::Float3{ points[2], depth },
				Math::Float3{ points[3], depth },
			},
			.Colors = std::vector(colors.begin(), colors.end()),
			.Indices = { 0, 1, 2, 2, 3, 0 },
		};
	}

	std::array<Math::Float2, 3> MeshBuilder::GenerateTrianglePoints(const Math::Float2& v0, const Math::Float2& v1, const Math::Float2& v2)
	{
		return { v0, v1, v2 };
	}

	Mesh MeshBuilder::GenerateFilledTriangleMesh(const std::span<const Math::Float2, 3>& points, const std::span<const Math::Float4, 3>& colors, const float depth)
	{
		return Mesh {
			.Positions = std::vector{
				Math::Float3{ points[0], depth },
				Math::Float3{ points[1], depth },
				Math::Float3{ points[2], depth },
			},
			.Colors = std::vector(colors.begin(), colors.end()),
			.Indices = { 0, 1, 2 },
		};
	}

	Mesh MeshBuilder::GenerateOutlinedMesh(const std::span<const Math::Float2>& points, const std::span<const Math::Float4>& colors, float strokeWeight, StrokeJoin strokeJoin, uint32_t roundedJoinSegments, float depth)
	{
		std::vector<Math::Float3> meshPositions;
		std::vector<Math::Float4> meshColors;
		std::vector<uint32_t> meshIndices;

		for (size_t i = 0; i < points.size(); ++i)
		{
			const Math::Float2& p0 = points[(i - 1 + points.size()) % points.size()];
			const Math::Float2& p1 = points[i];
			const Math::Float2& p2 = points[(i + 1) % points.size()];

			// Compute the direction vectors
			const Math::Float2 dir1 = (p1 - p0).Normalized();
			const Math::Float2 dir2 = (p2 - p1).Normalized();

			const float cross = dir1.X * dir2.Y - dir1.Y * dir2.X;
			const float sign = (cross >= 0.0f) ? 1.0f : -1.0f;

			// Compute the normal vectors
			const Math::Float2 n1 = dir1.Perpendicular();
			const Math::Float2 n2 = dir2.Perpendicular();

			// Compute the bisector vector
			const Math::Float2 bisector = (n1 + n2).Normalized();
			const float angleCos = n1.Dot(bisector);
			const float miterLength = (strokeWeight * 0.5f) / angleCos;

			switch (strokeJoin)
			{
				case StrokeJoin::Miter:
				{
					const Math::Float2 outer = p1 + bisector * miterLength * sign;
					const Math::Float2 inner = p1 - bisector * miterLength * sign;

					meshPositions.emplace_back(outer, depth);
					meshPositions.emplace_back(inner, depth);

					meshColors.emplace_back(colors[i]);
					meshColors.emplace_back(colors[i]);
				} break;

				default: ThrowError("Unknown StrokeJoin in MeshBuilder::GenerateOutlinedMesh()");
			}
		}

		for (size_t i = 0; i < points.size(); ++i)
		{
			const size_t nextIndex = (i + 1) % points.size();
			const uint32_t currentOuterIndex = static_cast<uint32_t>(i * 2);
			const uint32_t currentInnerIndex = static_cast<uint32_t>(i * 2 + 1);
			const uint32_t nextOuterIndex = static_cast<uint32_t>(nextIndex * 2);
			const uint32_t nextInnerIndex = static_cast<uint32_t>(nextIndex * 2 + 1);

			// First triangle
			meshIndices.emplace_back(currentOuterIndex);
			meshIndices.emplace_back(currentInnerIndex);
			meshIndices.emplace_back(nextOuterIndex);
			
			// Second triangle
			meshIndices.emplace_back(currentInnerIndex);
			meshIndices.emplace_back(nextInnerIndex);
			meshIndices.emplace_back(nextOuterIndex);
		}

		return Mesh {
			.Positions = std::move(meshPositions),
			.Colors = std::move(meshColors),
			.Indices = std::move(meshIndices)
		};
	}
}
