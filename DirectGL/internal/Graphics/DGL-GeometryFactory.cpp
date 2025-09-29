module;

#include <cmath>
#include <numbers>
#include <algorithm>

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
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },
			},
			.Indices = { 0, 1, 2, 2, 3, 0 },
		};
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
		const float angleStepSize = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);
		const float halfThickness = thickness * 0.5f;

		Geometry geometry;

		for (uint32_t segment = 0; segment <= segments; ++segment)
		{
			// Compute the 2D coordinate on screen
			const float angle = static_cast<float>(segment) * angleStepSize;
			const float cosine = std::cos(angle);
			const float sine = std::sin(angle);

			// Inner point
			{
				const float pointX = center.X + cosine * (radius.X - halfThickness);
				const float pointY = center.Y + sine * (radius.Y - halfThickness);
				geometry.Positions.emplace_back(pointX, pointY);

				const float texCoordX = 0.5f + cosine * 0.5f;
				const float texCoordY = 0.5f + sine * 0.5f;
				geometry.TexCoords.emplace_back(texCoordX, texCoordY);
			}

			// Outer point
			{
				const float pointX = center.X + cosine * (radius.X + halfThickness);
				const float pointY = center.Y + sine * (radius.Y + halfThickness);
				geometry.Positions.emplace_back(pointX, pointY);

				const float texCoordX = 0.5f + cosine * 0.5f;
				const float texCoordY = 0.5f + sine * 0.5f;
				geometry.TexCoords.emplace_back(texCoordX, texCoordY);
			}
		}

		// Generate the indices to act like a Triangle-Strip
		const size_t vertexCount = geometry.Positions.size();
		for (size_t i = 0; i < vertexCount - 2; i += 2)
		{
			geometry.Indices.emplace_back(static_cast<uint32_t>(i));       //< Outer current point
			geometry.Indices.emplace_back(static_cast<uint32_t>(i + 1));   //< Inner current point
			geometry.Indices.emplace_back(static_cast<uint32_t>(i + 2));   //< Outer next point
			geometry.Indices.emplace_back(static_cast<uint32_t>(i + 2));   //< Outer next point
			geometry.Indices.emplace_back(static_cast<uint32_t>(i + 1));   //< Inner current point
			geometry.Indices.emplace_back(static_cast<uint32_t>(i + 3));   //< Inner next point
		}

		return geometry;
	}
}