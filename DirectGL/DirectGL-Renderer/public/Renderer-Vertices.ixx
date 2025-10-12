// Project Name : DirectGL-Renderer
// File Name    : Renderer-Vertices.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <vector>

export module DirectGL.Renderer:Vertices;

import DirectGL.Math;

export namespace DGL::Renderer
{
	struct Vertices
	{
		std::vector<Math::Float2> Positions;	// list of positions - (x, y) in pairs
		std::vector<Math::Float2> TexCoords;	// list of texture coordinates - (u, v) in pairs
		std::vector<uint32_t> Indices;			// list of vertex indices - triangles in triplets
	};
}