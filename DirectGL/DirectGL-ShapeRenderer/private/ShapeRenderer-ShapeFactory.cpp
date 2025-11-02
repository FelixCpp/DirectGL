module;

#include <cmath>
#include <algorithm>
#include <vector>

module DirectGL.ShapeRenderer;

namespace DGL::ShapeRenderer
{
	Vertices ShapeFactory::GetFilledRectangle(const Math::FloatBoundary& boundary, const float depth)
	{
		Vertices vertices;
		vertices.Type = PrimitiveType::TriangleFan;

		vertices.Positions.reserve(4);
		vertices.Positions.emplace_back(boundary.Left, boundary.Top, depth);
		vertices.Positions.emplace_back(boundary.Right(), boundary.Top, depth);
		vertices.Positions.emplace_back(boundary.Right(), boundary.Bottom(), depth);
		vertices.Positions.emplace_back(boundary.Left, boundary.Bottom(), depth);

		return vertices;
	}

	Vertices ShapeFactory::GetOutlinedRectangle(const Math::FloatBoundary& boundary, const float strokeWeight, const float depth)
	{
		Vertices vertices;
		vertices.Type = PrimitiveType::Triangles;

		const float halfStroke = strokeWeight * 0.5f;
		const auto innerBoundary = Math::FloatBoundary::FromLTWH(boundary.Left + halfStroke, boundary.Top + halfStroke, boundary.Width - strokeWeight, boundary.Height - strokeWeight);
		const auto outerBoundary = Math::FloatBoundary::FromLTWH(boundary.Left - halfStroke, boundary.Top - halfStroke, boundary.Width + strokeWeight, boundary.Height + strokeWeight);

		vertices.Positions.reserve(8); // Each corner has an inner and outer vertex

		// Inner rectangle vertices
		vertices.Positions.emplace_back(innerBoundary.Left, innerBoundary.Top, depth);			// 0: Top-left
		vertices.Positions.emplace_back(innerBoundary.Right(), innerBoundary.Top, depth);		// 1: Top-right
		vertices.Positions.emplace_back(innerBoundary.Right(), innerBoundary.Bottom(), depth);	// 2: Bottom-right
		vertices.Positions.emplace_back(innerBoundary.Left, innerBoundary.Bottom(), depth);		// 3: Bottom-left

		// Outer rectangle vertices
		vertices.Positions.emplace_back(outerBoundary.Left, outerBoundary.Top, depth);			// 4: Top-left
		vertices.Positions.emplace_back(outerBoundary.Right(), outerBoundary.Top, depth);		// 5: Top-right
		vertices.Positions.emplace_back(outerBoundary.Right(), outerBoundary.Bottom(), depth);	// 6: Bottom-right
		vertices.Positions.emplace_back(outerBoundary.Left, outerBoundary.Bottom(), depth);		// 7: Bottom-left

		// Generate the indices for the outline (two triangles per side)
		vertices.Indices.reserve(24); // 4 sides * 2 triangles * 3 indices
		for (uint32_t i = 0; i < 4; i++)
		{
			const uint32_t innerCurrent = i;
			const uint32_t innerNext = (i + 1) % 4;
			const uint32_t outerCurrent = i + 4;
			const uint32_t outerNext = ((i + 1) % 4) + 4;
			// First triangle of the quad
			vertices.Indices.emplace_back(innerCurrent);
			vertices.Indices.emplace_back(outerCurrent);
			vertices.Indices.emplace_back(innerNext);
			// Second triangle of the quad
			vertices.Indices.emplace_back(outerCurrent);
			vertices.Indices.emplace_back(outerNext);
			vertices.Indices.emplace_back(innerNext);
		}

		return vertices;
	}

	Vertices ShapeFactory::GetFilledEllipse(const Math::Float2 center, const Math::Radius radius, const size_t segments, const float depth)
	{
		Vertices vertices;
		vertices.Type = PrimitiveType::TriangleFan;
		vertices.Positions.reserve(segments);

		for (size_t i = 0; i <= segments; i++)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float x = center.X + radius.X * std::cos(angle);
			const float y = center.Y + radius.Y * std::sin(angle);
			vertices.Positions.emplace_back(x, y, depth);
		}

		return vertices;
	}

	Vertices ShapeFactory::GetOutlinedEllipse(const Math::Float2 center, const Math::Radius radius, const size_t segments, const float strokeWeight, const float depth)
	{
		Vertices vertices;
		vertices.Type = PrimitiveType::Triangles;
		vertices.Positions.reserve(segments * 2); // Each segment has an inner and outer vertex
		const float halfStroke = strokeWeight * 0.5f;
		const auto innerRadius = Math::Radius::Elliptical(radius.X - halfStroke, radius.Y - halfStroke);
		const auto outerRadius = Math::Radius::Elliptical(radius.X + halfStroke, radius.Y + halfStroke);

		for (size_t i = 0; i < segments; i++)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float cosAngle = std::cos(angle);
			const float sinAngle = std::sin(angle);

			// Inner vertex
			const float innerX = center.X + innerRadius.X * cosAngle;
			const float innerY = center.Y + innerRadius.Y * sinAngle;
			vertices.Positions.emplace_back(innerX, innerY, depth);

			// Outer vertex
			const float outerX = center.X + outerRadius.X * cosAngle;
			const float outerY = center.Y + outerRadius.Y * sinAngle;
			vertices.Positions.emplace_back(outerX, outerY, depth);
		}

		// Generate the indices
		vertices.Indices.reserve(segments * 6); // Each segment forms two triangles
		for (size_t i = 0; i < segments; i++)
		{
			const size_t innerCurrent = i * 2;
			const size_t outerCurrent = innerCurrent + 1;

			const size_t innerNext = (i * 2 + 2) % (segments * 2);
			const size_t outerNext = (i * 2 + 3) % (segments * 2);

			// First triangle of the quad
			vertices.Indices.emplace_back(innerCurrent);
			vertices.Indices.emplace_back(outerCurrent);
			vertices.Indices.emplace_back(innerNext);

			// Second triangle of the quad
			vertices.Indices.emplace_back(outerCurrent);
			vertices.Indices.emplace_back(outerNext);
			vertices.Indices.emplace_back(innerNext);
		}

		return vertices;
	}

	Vertices ShapeFactory::GetFilledTriangle(const Math::Float2 a, const Math::Float2 b, const Math::Float2 c, const float depth)
	{
		Vertices vertices;
		vertices.Type = PrimitiveType::Triangles;

		vertices.Positions.reserve(3);
		vertices.Positions.emplace_back(a.X, a.Y, depth);
		vertices.Positions.emplace_back(b.X, b.Y, depth);
		vertices.Positions.emplace_back(c.X, c.Y, depth);

		return vertices;
	}

	Vertices ShapeFactory::GetOutlinedTriangle(const Math::Float2 a, const Math::Float2 b, const Math::Float2 c, const float strokeWeight, const LineJoinStyle joinStyle, const float depth)
	{
		return Vertices {
			.Type = PrimitiveType::Triangles,
			.Positions = {},
			.Indices = {},
		};
	}

	void AddButtLineCap(std::vector<Math::Float3>& positions, const Math::Float2 root, const Math::Float2 offset, const float depth)
	{
		// A butt cap does nothing more than the line itself.

		// Add the two positions for the butt cap.
		positions.reserve(positions.size() + 2);

		positions.emplace_back(root - offset, depth);
		positions.emplace_back(root + offset, depth);
	}

	void AddSquareLineCap(std::vector<Math::Float3>& positions, const Math::Float2 root, const Math::Float2 direction, const Math::Float2 offset, const float strokeWeight, const float depth)
	{
		// A square cap extends the line by half the stroke weight in the direction of the line.
		const Math::Float2 extension = direction * strokeWeight;

		// Add the two positions for the square cap.
		positions.reserve(positions.size() + 2);

		positions.emplace_back(root - offset + extension, depth);
		positions.emplace_back(root + offset + extension, depth);
	}

	void AddRoundLineCap(std::vector<Math::Float3>& positions, const Math::Float2 root, const Math::Angle startAngle, const Math::Angle sweepAngle, const float strokeWeight, const size_t segments, const float depth)
	{
		const size_t steps = std::max(2uz, segments);

		const float halfStrokeWeight = strokeWeight * 0.5f;
		const float startRadians = startAngle.AsRadians();
		const float sweepRadians = sweepAngle.AsRadians();

		const float step = sweepRadians / static_cast<float>(steps - 1);

		positions.reserve(positions.size() + steps);

		for (size_t i = 0; i < steps; ++i)
		{
			const float angle = startRadians + static_cast<float>(i) * step;
			const float x = root.X + std::cos(angle) * halfStrokeWeight;
			const float y = root.Y + std::sin(angle) * halfStrokeWeight;
			positions.emplace_back(x, y, depth);
		}
	}

	Vertices ShapeFactory::GetLine(const Math::Float2 start, const Math::Float2 end, const float strokeWeight, const LineCapStyle startCap, const LineCapStyle endCap, const std::function<size_t(Math::Radius)>& getSegmentCountForRoundCap, const float depth)
	{
		// Regardless of the line cap style, we need to compute the direction and perpendicular vectors.
		const Math::Float2 direction = (end - start).Normalized();
		const Math::Float2 perpendicular = direction.Perpendicular();
		const Math::Float2 offset = perpendicular * strokeWeight * 0.5f;
		const Math::Angle lineAngle = direction.Heading();

		const size_t segmentCountForRoundCap = (startCap == LineCapStyle::Round) or (endCap == LineCapStyle::Round)
			? getSegmentCountForRoundCap(Math::Radius::Circular(strokeWeight * 0.5f))
			: 0;

		// First step is to compute the positions for the start of the line.
		// These positions depend on the line start cap style.
		std::vector<Math::Float3> positions;

		switch (startCap)
		{
			case LineCapStyle::Butt: AddButtLineCap(positions, start, offset, depth); break;
			case LineCapStyle::Square: AddSquareLineCap(positions, start, -direction, offset, strokeWeight, depth); break;
			case LineCapStyle::Round: AddRoundLineCap(positions, start, lineAngle + Math::Degrees(270.0f), Math::Degrees(-180.0f), strokeWeight, segmentCountForRoundCap, depth); break;
		}

		const size_t startPositionCount = positions.size(); //< Remember how many positions we have after the start cap.

		// Now add the positions for the end of the line.
		switch (endCap)
		{
			case LineCapStyle::Butt: AddButtLineCap(positions, end, offset, depth); break;
			case LineCapStyle::Square: AddSquareLineCap(positions, end, direction, offset, strokeWeight, depth); break;
			case LineCapStyle::Round: AddRoundLineCap(positions, end, lineAngle - Math::Degrees(90.0f), Math::Degrees(180.0f), strokeWeight, segmentCountForRoundCap, depth); break;
		}

		const size_t endPositionCount = positions.size() - startPositionCount; //< Remember how many positions we have after the end cap.

		// At this point, we have gathered all positions for the line.
		//
		// The following graphic illustrates how the positions are organized:
		//
		// In case of a line with butt or square caps:
		// 0 ------------------ 2
		// |                    |
		// |                    |
		// |                    |
		// 1 ------------------ 3
		//
		// In case of round caps, there will be more positions at the start and/or end of the line:
		//     0 ------------------ 7
		//    1						|
		//   2						|
		//  3						|
		//   4						|
		//    5						|
		//     6 ------------------ 8
		//
		// In this case we iterate over all positions and get always a pair of positions:
		// For the start cap, we get the first N positions (0 to startPositionCount - 1)
		// For the end cap, we get the last N positions (startPositionCount to total count - 1)
		// We then create a triangle strip by connecting these pairs.

		std::vector<uint32_t> indices;

		const size_t maximumIterations = std::max(startPositionCount, endPositionCount);
		for (size_t i = 0; i < maximumIterations; ++i)
		{
			const size_t startPositionIndex = std::min(i, startPositionCount - 1); // Get the index for the start cap positions
			const size_t endPositionIndex = startPositionCount + std::min(i, endPositionCount - 1); // Get the index for the end cap positions

			indices.emplace_back(startPositionIndex);
			indices.emplace_back(endPositionIndex);
		}

		return Vertices {
			.Type = PrimitiveType::TriangleStrip,
			.Positions = std::move(positions),
			.Indices = std::move(indices),
		};
	}
}