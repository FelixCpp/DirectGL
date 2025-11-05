// Project Name : DirectGL
// File Name    : DirectGL-Mesh.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <vector>

export module DirectGL:Mesh;

import :Color;

namespace DGL
{
	/**
	 * This structure represents a mesh consisting of vertices (positions, colors) and indices.
	 * The data contained within the structure are meant to be rendered as individual
	 * triangles.
	 *
	 * Note that there has to be a 1:1 correspondence between the number of positions
	 * and colors; each position must have an associated color.
	 */
	struct Mesh
	{
		/**
		 * The positions of each vertex in the mesh.
		 */
		std::vector<Math::Float3> Positions;

		/**
		 * The colors associated with each vertex in the mesh.
		 */
		std::vector<Math::Float4> Colors;

		/**
		 * Indices into the vertex array that define the triangles
		 * of the mesh.
		 */
		std::vector<uint32_t> Indices;
	};
}