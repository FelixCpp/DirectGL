module;

#include <cmath>
#include <numbers>

module DGL;

namespace DGL
{
	Geometry GeometryFactory::CreateFilledRectangle(const float left, const float top, const float width, const float height)
	{
		return {
			.Positions = {
				{ left, top },
				{ left + width, top },
				{ left + width, top + height },
				{ left, top + height },
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

	Geometry GeometryFactory::CreateFilledEllipse(const float centerX, const float centerY, const float radiusX, const float radiusY, const uint32_t segments)
	{
		if (segments < 3) return {};

		const float minX = centerX - radiusX;
		const float maxX = centerX + radiusX;
		const float minY = centerY - radiusY;
		const float maxY = centerY + radiusY;

		const float width = maxX - minX;
		const float height = maxY - minY;

		Geometry geometry;

		float angle = 0.0f;
		const float angleStepSize = (2.0f * std::numbers::pi_v<float>) / static_cast<float>(segments);
		for (uint32_t i = 0; i < segments; ++i)
		{
			const float x = centerX + std::cos(angle) * radiusX;
			const float y = centerY + std::sin(angle) * radiusY;
			geometry.Positions.emplace_back(x, y);

			const float tx = (x - minX) / width;
			const float ty = 1.0f - (y - minY) / height;
			geometry.TexCoords.emplace_back(tx, ty);

			angle += angleStepSize;
		}

		for (uint32_t i = 0; i < segments; ++i)
		{
			geometry.Indices.push_back(i);
			geometry.Indices.push_back((i + 1) % segments);
			geometry.Indices.push_back(segments); // Center vertex
		}

		return geometry;
	}
}
