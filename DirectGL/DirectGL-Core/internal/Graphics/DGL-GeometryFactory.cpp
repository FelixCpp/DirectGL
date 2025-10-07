module;

#include <cmath>
#include <numbers>
#include <algorithm>
#include <format>

module DGL;

namespace DGL
{
	Geometry GeometryFactory::CreateFilledRectangle(const Math::FloatBoundary& boundary)
	{
		return {
			.Positions = {
				boundary.TopLeft(),
				boundary.TopRight(),
				boundary.BottomRight(),
				boundary.BottomLeft(),
			},
			.TexCoords = {
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },
				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f },
			},
			.Indices = { 0, 1, 2, 2, 3, 0 },
		};
	}

	Geometry GeometryFactory::CreateOutlinedRectangle(const Math::FloatBoundary& boundary, float thickness)
	{
		if (thickness <= 0.0f) return {};

		Geometry geometry;

		// Outer rectangle
		geometry.Positions.emplace_back(boundary.TopLeft());
		geometry.Positions.emplace_back(boundary.TopRight());
		geometry.Positions.emplace_back(boundary.BottomRight());
		geometry.Positions.emplace_back(boundary.BottomLeft());

		// Inner rectangle
		geometry.Positions.emplace_back(boundary.TopLeft() + Math::Float2{ thickness, thickness });
		geometry.Positions.emplace_back(boundary.TopRight() + Math::Float2{ -thickness, thickness });
		geometry.Positions.emplace_back(boundary.BottomRight() + Math::Float2{ -thickness, -thickness });
		geometry.Positions.emplace_back(boundary.BottomLeft() + Math::Float2{ thickness, -thickness });

		// Texture coordinates for outer rectangle
		geometry.TexCoords.emplace_back(0.0f, 0.0f);
		geometry.TexCoords.emplace_back(1.0f, 0.0f);
		geometry.TexCoords.emplace_back(1.0f, 1.0f);
		geometry.TexCoords.emplace_back(0.0f, 1.0f);

		// Texture coordinates for inner rectangle
		const float innerLeft = thickness / boundary.Width;
		const float innerTop = thickness / boundary.Height;
		const float innerRight = 1.0f - innerLeft;
		const float innerBottom = 1.0f - innerTop;
		geometry.TexCoords.emplace_back(innerLeft, innerTop);
		geometry.TexCoords.emplace_back(innerRight, innerTop);
		geometry.TexCoords.emplace_back(innerRight, innerBottom);
		geometry.TexCoords.emplace_back(innerLeft, innerBottom);

		// Indices for the outline (two triangles per side)
		for (uint32_t i = 0; i < 4; ++i)
		{
			const uint32_t nextI = (i + 1) % 4;

			constexpr uint32_t outerStartIndex = 0;
			constexpr uint32_t innerStartIndex = 4;

			// First triangle of the side
			geometry.Indices.emplace_back(outerStartIndex + i);
			geometry.Indices.emplace_back(outerStartIndex + nextI);
			geometry.Indices.emplace_back(innerStartIndex + nextI);

			// Second triangle of the side
			geometry.Indices.emplace_back(innerStartIndex + nextI);
			geometry.Indices.emplace_back(innerStartIndex + i);
			geometry.Indices.emplace_back(outerStartIndex + i);
		}

		return geometry;
	}


	Geometry GeometryFactory::CreateFilledRoundedRectangle(
		const Math::FloatBoundary& boundary,
		const BorderRadius& borderRadius,
		const uint32_t segmentsTopLeft,
		const uint32_t segmentsTopRight,
		const uint32_t segmentsBottomRight,
		const uint32_t segmentsBottomLeft
	) {
		Geometry geometry;

		const auto addCorner = [&geometry, &boundary](const Math::Float2& cornerCenter, const Radius& cornerRadius, float startAngle, float endAngle, uint32_t segments)
		{
			if (segments <= 1 or (cornerRadius.X <= 0.0f and cornerRadius.Y <= 0.0f))
			{
				// No rounded corner, just add the corner point
				geometry.Positions.emplace_back(cornerCenter);

				const float texCoordX = (cornerCenter.X - boundary.Left) / boundary.Width;
				const float texCoordY = (cornerCenter.Y - boundary.Top) / boundary.Height;
				geometry.TexCoords.emplace_back(texCoordX, texCoordY);

				return;
			}

			constexpr float DEG_TO_RAD = std::numbers::pi_v<float> / 180.0f;

			for (uint32_t segment = 0; segment <= segments; ++segment)
			{
				const float t = static_cast<float>(segment) / static_cast<float>(segments);
				const float angle = std::lerp(startAngle, endAngle, t) * DEG_TO_RAD;

				const float pointX = cornerCenter.X + std::cos(angle) * cornerRadius.X;
				const float pointY = cornerCenter.Y + std::sin(angle) * cornerRadius.Y;
				geometry.Positions.emplace_back(pointX, pointY);

				const float texCoordX = (pointX - boundary.Left) / boundary.Width;
				const float texCoordY = (pointY - boundary.Top) / boundary.Height;
				geometry.TexCoords.emplace_back(texCoordX, texCoordY);
			}
		};

		// Insert center point for triangle-fan
		geometry.Positions.emplace_back(boundary.Center());
		geometry.TexCoords.emplace_back(0.5f, 0.5f);

		const Math::Float2 topLeft = boundary.TopLeft() + Math::Float2{ borderRadius.TopLeft.X, borderRadius.TopLeft.Y };
		const Math::Float2 topRight = boundary.TopRight() + Math::Float2{ -borderRadius.TopRight.X, borderRadius.TopRight.Y };
		const Math::Float2 bottomRight = boundary.BottomRight() + Math::Float2{ -borderRadius.BottomRight.X, -borderRadius.BottomRight.Y };
		const Math::Float2 bottomLeft = boundary.BottomLeft() + Math::Float2{ borderRadius.BottomLeft.X, -borderRadius.BottomLeft.Y };

		addCorner(topLeft, borderRadius.TopLeft, 180.0f, 270.0f, segmentsTopLeft);
		addCorner(topRight, borderRadius.TopRight, 270.0f, 360.0f, segmentsTopRight);
		addCorner(bottomRight, borderRadius.BottomRight, 0.0f, 90.0f, segmentsBottomRight);
		addCorner(bottomLeft, borderRadius.BottomLeft, 90.0f, 180.0f, segmentsBottomLeft);

		// Insert indices
		const size_t vertexCount = geometry.Positions.size();
		for (size_t i = 1; i < vertexCount; ++i)
		{
			size_t nextIndex = i + 1;
			if (nextIndex == vertexCount) nextIndex = 1; // Wrap around to the first corner point

			geometry.Indices.emplace_back(0); //< Center point
			geometry.Indices.emplace_back(i); //< Current point
			geometry.Indices.emplace_back(nextIndex); //< Next point (wrap around)
		}

		return geometry;
	}

	Geometry GeometryFactory::CreateFilledEllipse(const Math::Float2& center, const Radius& radius, const uint32_t segments)
	{
		if (segments < 3) return {};

		const float angleStepSize = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);

		Geometry geometry;

		// Insert the center point first
		geometry.Positions.emplace_back(center);
		geometry.TexCoords.emplace_back(0.5f, 0.5f);

		// Now iterate over the segments and create the ellipse points
		for (uint32_t segment = 0; segment <= segments; ++segment)
		{
			// Compute the 2D coordinate on screen
			const float angle = static_cast<float>(segment) * angleStepSize;
			const float cosine = std::cos(angle);
			const float sine = std::sin(angle);

			const float pointX = center.X + cosine * radius.X;
			const float pointY = center.Y + sine * radius.Y;
			geometry.Positions.emplace_back(pointX, pointY);

			// Compute the normalized texture coordinate used when fetching colors from a texture
			const float texCoordX = 0.5f + cosine * 0.5f;
			const float texCoordY = 0.5f + sine * 0.5f;
			geometry.TexCoords.emplace_back(texCoordX, texCoordY);
		}

		// Generate the indices to act like a Triangle-Fan
		for (size_t i = 1; i <= segments; ++i)
		{
			geometry.Indices.emplace_back(0); //< Center point
			geometry.Indices.emplace_back(i); //< Current point
			geometry.Indices.emplace_back(i + 1); //< Next point
		}

		return geometry;
	}

	Geometry GeometryFactory::CreateOutlinedEllipse(const Math::Float2& center, const Radius& radius, const uint32_t segments, const float thickness)
	{
		if (segments < 3 || thickness <= 0.0f) return {};

		Geometry geometry;

		const float angleStepSize = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);

		// Erzeuge äußeren und inneren Rand
		for (uint32_t segment = 0; segment <= segments; ++segment)
		{
			const float angle = static_cast<float>(segment) * angleStepSize;
			const float cosine = std::cos(angle);
			const float sine = std::sin(angle);

			// Äußerer Punkt
			const float outerX = center.X + cosine * radius.X;
			const float outerY = center.Y + sine * radius.Y;
			geometry.Positions.emplace_back(outerX, outerY);
			geometry.TexCoords.emplace_back(0.5f + cosine * 0.5f, 0.5f + sine * 0.5f);

			// Innerer Punkt
			const float innerX = center.X + cosine * (radius.X - thickness);
			const float innerY = center.Y + sine * (radius.Y - thickness);
			geometry.Positions.emplace_back(innerX, innerY);
			geometry.TexCoords.emplace_back(
				0.5f + cosine * 0.5f * ((radius.X - thickness) / radius.X),
				0.5f + sine * 0.5f * ((radius.Y - thickness) / radius.Y)
			);
		}

		// Indizes für das Triangle Strip
		for (uint32_t i = 0; i < segments; ++i)
		{
			uint32_t outer0 = i * 2;
			uint32_t inner0 = i * 2 + 1;
			uint32_t outer1 = (i + 1) * 2;
			uint32_t inner1 = (i + 1) * 2 + 1;

			geometry.Indices.emplace_back(outer0);
			geometry.Indices.emplace_back(inner0);
			geometry.Indices.emplace_back(outer1);

			geometry.Indices.emplace_back(outer1);
			geometry.Indices.emplace_back(inner0);
			geometry.Indices.emplace_back(inner1);
		}

		return geometry;
	}

	Geometry GeometryFactory::CreateLine(const Math::Float2& start, const Math::Float2& end, const float thickness, const LineCap startCap, const LineCap endCap)
	{
		Geometry geometry;

		// Compute the direction vector of the line and its perpendicular to form a rectangle
		const Math::Float2 difference = end - start;
		const Math::Float2 direction = difference.Normalized();
		const Math::Float2 perpendicular = direction.Perpendicular();
		const Math::Float2 offset = perpendicular * (thickness * 0.5f);

		std::vector<Math::Float2> startPoints;
		std::vector<Math::Float2> startTexCoords;
		switch (startCap)
		{
			case LineCap::Butt:
			{
				// Add two points for the start of the line
				startPoints.emplace_back(start - offset);
				startPoints.emplace_back(start + offset);

				// Insert texture coordinates for the start points
				startTexCoords.emplace_back(0.0f, 0.0f);
				startTexCoords.emplace_back(0.0f, 1.0f);
			} break;

			case LineCap::Round:
			{
				// Generate a half-circle around the start point pointing backwards along the line direction
				constexpr size_t segments = 4;
				constexpr float angleStepSize = std::numbers::pi_v<float> / static_cast<float>(segments - 1);
				const float startAngle = std::atan2(offset.Y, offset.X) + std::numbers::pi_v<float>;
				const float radius = thickness * 0.5f;
				for (size_t i = 0; i < segments; ++i)
				{
					const float angle = startAngle - static_cast<float>(i) * angleStepSize;
					//Trace(std::format("Angle[{}] = {:.2f} deg", i, angle * 180.0f / 3.141592f));
					const float px = start.X + std::cos(angle) * radius;
					const float py = start.Y + std::sin(angle) * radius;
					startPoints.emplace_back(px, py);

					// Insert texture coordinates for the start points
					const float texCoordX = 0.5f + (std::cos(angle) * 0.5f);
					const float texCoordY = 0.5f + (std::sin(angle) * 0.5f);
					startTexCoords.emplace_back(texCoordX, texCoordY);
				}
			} break;

			case LineCap::Square:
			{
				// Add two points at the start of the line, offset by half the thickness in the opposite line direction
				const Math::Float2 squareOffset = direction * (thickness * 0.5f);
				startPoints.emplace_back(start - offset - squareOffset);
				startPoints.emplace_back(start + offset - squareOffset);
			} break;
		}

		std::vector<Math::Float2> endPoints;
		std::vector<Math::Float2> endTexCoords;
		switch (endCap)
		{
			case LineCap::Butt:
			{
				// Add two points for the end of the line
				endPoints.emplace_back(end - offset);
				endPoints.emplace_back(end + offset);

				// Insert texture coordinates for the end points
				endTexCoords.emplace_back(1.0f, 0.0f);
				endTexCoords.emplace_back(1.0f, 1.0f);
			} break;

			case LineCap::Round:
			{
				// Generate a half-circle around the end point pointing forwards along the line direction
				constexpr size_t segments = 4;
				constexpr float angleStepSize = std::numbers::pi_v<float> / static_cast<float>(segments - 1);
				const float startAngle = std::atan2(offset.Y, offset.X) + std::numbers::pi_v<float>;
				const float radius = thickness * 0.5f;
				for (size_t i = 0; i < segments; ++i)
				{
					const float angle = startAngle + static_cast<float>(i) * angleStepSize;
					//Trace(std::format("Angle[{}] = {:.2f} deg", i, angle * 180.0f / 3.141592f));
					const float px = end.X + std::cos(angle) * radius;
					const float py = end.Y + std::sin(angle) * radius;
					endPoints.emplace_back(px, py);

					// Insert texture coordinates for the end points
					const float texCoordX = 0.5f + (std::cos(angle) * 0.5f);
					const float texCoordY = 0.5f + (std::sin(angle) * 0.5f);
					endTexCoords.emplace_back(texCoordX, texCoordY);
				}
			} break;

			case LineCap::Square:
			{
				// Add two points at the end of the line, offset by half the thickness in the line direction
				const Math::Float2 squareOffset = direction * (thickness * 0.5f);
				endPoints.emplace_back(end - offset + squareOffset);
				endPoints.emplace_back(end + offset + squareOffset);
			} break;
		}

		geometry.Positions.append_range(startPoints);
		geometry.Positions.append_range(endPoints);

		geometry.TexCoords.append_range(std::move(startTexCoords));
		geometry.TexCoords.append_range(std::move(endTexCoords));
		
		// Generate the indices for the triangle strip
		std::vector<uint32_t> triangleStripIndices;
		const size_t startCount = startPoints.size();
		const size_t endCount = endPoints.size();
		const size_t higherPointCount = std::max(startCount, endCount);
		for (size_t i = 0; i < higherPointCount; ++i)
		{
			const size_t startIdx = std::min(i, startCount - 1);
			const size_t endIdx = std::min(i, endCount - 1);
		
			triangleStripIndices.emplace_back(static_cast<uint32_t>(startIdx));
			triangleStripIndices.emplace_back(static_cast<uint32_t>(startCount + endIdx));
		}
		
		// Convert triangle strip indices to triangle list indices
		for (size_t i = 0; i < triangleStripIndices.size() - 2; ++i)
		{
			if (i % 2 == 0)
			{
				geometry.Indices.emplace_back(triangleStripIndices[i]);
				geometry.Indices.emplace_back(triangleStripIndices[i + 1]);
				geometry.Indices.emplace_back(triangleStripIndices[i + 2]);
			}
			else
			{
				geometry.Indices.emplace_back(triangleStripIndices[i + 1]);
				geometry.Indices.emplace_back(triangleStripIndices[i]);
				geometry.Indices.emplace_back(triangleStripIndices[i + 2]);
			}
		}

		return geometry;
	}

}