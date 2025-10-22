module;

#include <cmath>
#include <algorithm>

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

	Vertices ShapeFactory::GetLine(const Math::Float2 start, const Math::Float2 end, const float strokeWeight, LineCapStyle startCap, LineCapStyle endCap, const float depth)
	{
		Vertices vertices;

		return vertices;
	}
}