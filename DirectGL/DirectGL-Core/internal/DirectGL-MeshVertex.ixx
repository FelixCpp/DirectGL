// Project Name : DirectGL
// File Name    : DirectGL-MeshVertex.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

export module DirectGL:MeshVertex;

import DirectGL.Math;

namespace DGL
{
	/**
	 * A MeshVertex represents a single vertex in 2D space with
	 * position and color attributes.
	 */
	struct MeshVertex
	{
		/**
		 * A 2D position of the vertex.
		 */
		Math::Float2 Position;

		/**
		 * The color of the vertex in RGBA format.
		 */
		Math::Float4 Color;
	};
}