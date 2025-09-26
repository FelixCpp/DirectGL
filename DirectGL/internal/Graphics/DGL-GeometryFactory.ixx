// Project Name : DirectGL
// File Name    : DGL-GeometryFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:GeometryFactory;

import :Geometry;
import :Radius;

import Math;

namespace DGL
{
	struct GeometryFactory
	{
		static Geometry CreateFilledRectangle(const Math::FloatBoundary& boundary);
		static Geometry CreateFilledRoundedRectangle(
			const Math::FloatBoundary& boundary,
			const BorderRadius& borderRadius,
			uint32_t segmentsTopLeft,
			uint32_t segmentsTopRight,
			uint32_t segmentsBottomRight,
			uint32_t segmentsBottomLeft
		);

		static Geometry CreateFilledEllipse(const Math::Float2& center, const Radius& radius, uint32_t segments);
	};
}