// Project Name : DirectGL
// File Name    : DGL-GeometryFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:GeometryFactory;

import :Geometry;

namespace DGL
{
	struct GeometryFactory
	{
		static Geometry CreateFilledRectangle(float left, float top, float width, float height);
		static Geometry CreateFilledEllipse(float centerX, float centerY, float radiusX, float radiusY, uint32_t segments);
	};
}