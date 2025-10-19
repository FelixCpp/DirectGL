// Project Name : DirectGL-ShapeRenderer
// File Name    : ShapeRenderer-Vertices.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

module;

#include <vector>

export module DirectGL.ShapeRenderer:Vertices;

import DirectGL.Math;

import :PrimitiveType;

export namespace DGL::ShapeRenderer
{
	struct Vertices
	{
		std::vector<Math::Float3>	Positions;	//!< List of vertex positions
		std::vector<uint32_t>		Indices;	//!< Indices into positions
		PrimitiveType				Type;		//!< Primitive type
	};
}