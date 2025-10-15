// Project Name : DirectGL-ShapeRenderer
// File Name    : ShapeRenderer-Vertices.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

module;

#include <vector>

export module DirectGL.ShapeRenderer:Vertices;

import DirectGL.Math;

export namespace DGL::ShapeRenderer
{
	struct Vertices
	{
		std::vector<Math::Float3> Positions; // x, y, z
		std::vector<uint32_t> Indices;
	};
}