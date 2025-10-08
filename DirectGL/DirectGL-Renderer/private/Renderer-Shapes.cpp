module;

#include <cmath>

module DirectGL.Renderer;

namespace DGL::Renderer
{
	Vertices GetFilledRectangle(const Math::FloatBoundary& boundary)
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

		return vertices;
	}

	Vertices GetFilledEllipse(const Math::Float2 center, const Math::Radius radius, const uint32_t segments)
	{
		Vertices vertices;
		vertices.Positions.reserve(segments + 1); // + 1 for the center point

		// Insert the center point in order to form a triangle fan
		vertices.Positions.emplace_back(center.X, center.Y);

		for (uint32_t i = 0; i < segments; i++)
		{
			const float angle = (static_cast<float>(i) / static_cast<float>(segments)) * Math::TAU;
			const float x = center.X + radius.X * std::cos(angle);
			const float y = center.Y + radius.Y * std::sin(angle);
			vertices.Positions.emplace_back(x, y);
		}

		// Generate the indices
		vertices.Indices.reserve(segments * 3); // Each segment forms a triangle with the center
		for (uint32_t i = 1; i <= segments; i++)
		{
			vertices.Indices.emplace_back(0); // Center point
			vertices.Indices.emplace_back(i); // Current point on the ellipse
			vertices.Indices.emplace_back(i % segments + 1); // Next point on the ellipse (wrap around)
		}

		return vertices;
	}

	Vertices GetOutlinedEllipse(const Math::Float2 center, const Math::Radius radius, const float strokeWeight, const uint32_t segments)
	{
		Vertices vertices;
		vertices.Positions.reserve(segments * 2); // Each segment has an inner and outer vertex
		const float halfStroke = strokeWeight * 0.5f;
		const auto innerRadius = Math::Radius::Elliptical(radius.X - halfStroke, radius.Y - halfStroke);
		const auto outerRadius = Math::Radius::Elliptical(radius.X + halfStroke, radius.Y + halfStroke);

		for (uint32_t i = 0; i < segments; i++)
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
		for (uint32_t i = 0; i < segments; i++)
		{
			const uint32_t innerCurrent = i * 2;
			const uint32_t outerCurrent = innerCurrent + 1;

			const uint32_t innerNext = (i * 2 + 2) % (segments * 2);
			const uint32_t outerNext = (i * 2 + 3) % (segments * 2);

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

	Vertices GetLine(const Math::Float2 start, const Math::Float2 end, const float strokeWeight)
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
		vertices.Positions.emplace_back(v0.X, v0.Y);
		vertices.Positions.emplace_back(v1.X, v1.Y);
		vertices.Positions.emplace_back(v2.X, v2.Y);
		vertices.Positions.emplace_back(v3.X, v3.Y);

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

	Vertices GetFilledTriangle(const Math::Float2 v1, const Math::Float2 v2, const Math::Float2 v3)
	{
		Vertices vertices;

		vertices.Positions.reserve(3);
		vertices.Positions.emplace_back(v1.X, v1.Y);
		vertices.Positions.emplace_back(v2.X, v2.Y);
		vertices.Positions.emplace_back(v3.X, v3.Y);

		vertices.Indices.reserve(3);
		vertices.Indices.emplace_back(0);
		vertices.Indices.emplace_back(1);
		vertices.Indices.emplace_back(2);

		return vertices;
	}
}