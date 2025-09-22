// Project Name : DirectGL
// File Name    : DGL-Geometry.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <vector>
#include <cstdint>

export module DGL:Geometry;

import Math;

namespace DGL
{
	struct Geometry
	{
		std::vector<Math::Float2> Positions;
		std::vector<Math::Float2> TexCoords;
		std::vector<uint32_t> Indices;
	};
}