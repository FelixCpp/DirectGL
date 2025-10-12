module;

#include <cmath>
#include <limits>
#include <algorithm>

module DirectGL.Renderer;

namespace DGL::Renderer
{
	inline void AddTextureCoords(Vertices& vertices)
	{
		float minX = std::numeric_limits<float>::max();
		float minY = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float maxY = std::numeric_limits<float>::min();

		for (const Math::Float2& position : vertices.Positions)
		{
			minX = std::min(minX, position.X);
			minY = std::min(minY, position.Y);
			maxX = std::max(maxX, position.X);
			maxY = std::max(maxY, position.Y);
		}

		const float width = maxX - minX;
		const float height = maxY - minY;

		vertices.TexCoords.resize(vertices.Positions.size());
		for (size_t i = 0; i < vertices.Positions.size(); ++i)
		{
			const Math::Float2& position = vertices.Positions[i];
			vertices.TexCoords[i].X = (position.X - minX) / width;
			vertices.TexCoords[i].Y = 1.0f - (position.Y - minY) / height;
		}
	}

	Vertices DefaultShapeFactory::GetFilledRectangle(const Math::FloatBoundary& boundary)
	{
		Vertices vertices;

		vertices.Positions.reserve(4);
		vertices.Positions.emplace_back(boundary.Left, boundary.Top);
		vertices.Positions.emplace_back(boundary.Right(), boundary.Top);
		vertices.Positions.emplace_back(boundary.Right(), boundary.Bottom());
		vertices.Positions.emplace_back(boundary.Left, boundary.Bottom());

		vertices.Indices.reserve(6);
		vertices.Indices.emplace_back(0);
		vertices.Indices.emplace_back(1);
		vertices.Indices.emplace_back(2);
		vertices.Indices.emplace_back(2);
		vertices.Indices.emplace_back(3);
		vertices.Indices.emplace_back(0);

		AddTextureCoords(vertices);

		return vertices;
	}

	Vertices DefaultShapeFactory::GetOutlinedRectangle(const Math::FloatBoundary& boundary, const float strokeWeight)
	{
		Vertices vertices;

		const float halfStroke = strokeWeight * 0.5f;
		const auto innerBoundary = Math::FloatBoundary::FromLTWH(boundary.Left + halfStroke, boundary.Top + halfStroke, boundary.Width - strokeWeight, boundary.Height - strokeWeight);
		const auto outerBoundary = Math::FloatBoundary::FromLTWH(boundary.Left - halfStroke, boundary.Top - halfStroke, boundary.Width + strokeWeight, boundary.Height + strokeWeight);

		vertices.Positions.reserve(8); // Each corner has an inner and outer vertex

		// Inner rectangle vertices
		vertices.Positions.emplace_back(innerBoundary.Left, innerBoundary.Top);       // 0: Top-left
		vertices.Positions.emplace_back(innerBoundary.Right(), innerBoundary.Top);    // 1: Top-right
		vertices.Positions.emplace_back(innerBoundary.Right(), innerBoundary.Bottom()); // 2: Bottom-right
		vertices.Positions.emplace_back(innerBoundary.Left, innerBoundary.Bottom());   // 3: Bottom-left

		// Outer rectangle vertices
		vertices.Positions.emplace_back(outerBoundary.Left, outerBoundary.Top);       // 4: Top-left
		vertices.Positions.emplace_back(outerBoundary.Right(), outerBoundary.Top);    // 5: Top-right
		vertices.Positions.emplace_back(outerBoundary.Right(), outerBoundary.Bottom()); // 6: Bottom-right
		vertices.Positions.emplace_back(outerBoundary.Left, outerBoundary.Bottom());   // 7: Bottom-left

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

	Vertices DefaultShapeFactory::GetFilledEllipse(const Math::Float2 center, const Math::Radius radius, const size_t segments)
	{
		Vertices vertices;
		vertices.Positions.reserve(segments + 1); // + 1 for the center point

		// Insert the center point in order to form a triangle fan
		vertices.Positions.emplace_back(center.X, center.Y);

		for (size_t i = 0; i < segments; i++)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float x = center.X + radius.X * std::cos(angle);
			const float y = center.Y + radius.Y * std::sin(angle);
			vertices.Positions.emplace_back(x, y);
		}

		// Generate the indices
		vertices.Indices.reserve(segments * 3); // Each segment forms a triangle with the center
		for (size_t i = 1; i <= segments; i++)
		{
			vertices.Indices.emplace_back(0); // Center point
			vertices.Indices.emplace_back(i); // Current point on the ellipse
			vertices.Indices.emplace_back(i % segments + 1); // Next point on the ellipse (wrap around)
		}

		return vertices;
	}

	Vertices DefaultShapeFactory::GetOutlinedEllipse(const Math::Float2 center, const Math::Radius radius, const size_t segments, const float strokeWeight)
	{
		Vertices vertices;
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
			vertices.Positions.emplace_back(innerX, innerY);

			// Outer vertex
			const float outerX = center.X + outerRadius.X * cosAngle;
			const float outerY = center.Y + outerRadius.Y * sinAngle;
			vertices.Positions.emplace_back(outerX, outerY);
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

	Vertices DefaultShapeFactory::GetFilledTriangle(const Math::Float2 a, const Math::Float2 b, const Math::Float2 c)
	{
		Vertices vertices;

		vertices.Positions.reserve(3);
		vertices.Positions.emplace_back(a);
		vertices.Positions.emplace_back(b);
		vertices.Positions.emplace_back(c);

		vertices.Indices.reserve(3);
		vertices.Indices.emplace_back(0);
		vertices.Indices.emplace_back(1);
		vertices.Indices.emplace_back(2);

		return vertices;
	}
	Vertices DefaultShapeFactory::GetLine(const Math::Float2 start, const Math::Float2 end, const float strokeWeight)
	{
		const Math::Float2 direction = (end - start).Normalized();
		const Math::Float2 perpendicular = direction.Perpendicular() * (strokeWeight * 0.5f);

		// Define the four corners of the rectangle
		const Math::Float2 v0 = start + perpendicular; // Top-left
		const Math::Float2 v1 = end + perpendicular;   // Top-right
		const Math::Float2 v2 = end - perpendicular;   // Bottom-right
		const Math::Float2 v3 = start - perpendicular; // Bottom-left

		Vertices vertices;

		vertices.Positions.reserve(4); // A line is represented as a rectangle (two triangles)
		vertices.Positions.emplace_back(v0);
		vertices.Positions.emplace_back(v1);
		vertices.Positions.emplace_back(v2);
		vertices.Positions.emplace_back(v3);

		// Define the two triangles that make up the rectangle
		vertices.Indices.reserve(6);
		vertices.Indices.emplace_back(0);
		vertices.Indices.emplace_back(1);
		vertices.Indices.emplace_back(2);
		vertices.Indices.emplace_back(2);
		vertices.Indices.emplace_back(3);
		vertices.Indices.emplace_back(0);

		return vertices;
	}
}