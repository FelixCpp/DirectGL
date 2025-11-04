// Project Name : DirectGL
// File Name    : DirectGL-Mesh.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <vector>

export module DirectGL:Mesh;

import :MeshVertex;

namespace DGL
{
	/**
	 * This structure represents a mesh consisting of vertices and indices.
	 * The data contained within the structure are meant to be rendered as individual
	 * triangles.
	 */
	struct Mesh
	{
		/**
		 * The vertices that make up the mesh.
		 */
		std::vector<MeshVertex>	Vertices;

		/**
		 * Indices into the vertex array that define the triangles
		 * of the mesh.
		 */
		std::vector<uint32_t>	Indices;

		/**
		 * The depth at which the mesh should be rendered.
		 * This is used for layering multiple meshes on top of
		 * each other.
		 */
		float Depth;
	};
}