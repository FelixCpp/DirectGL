// Project Name : DirectGL
// File Name    : DirectGL-MeshBuilder.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <cstdint>
#include <span>

export module DirectGL:MeshBuilder;

import DirectGL.Math;

import :Mesh;
import :Color;
import :StrokeJoin;
import :StrokeCap;

namespace DGL
{

	struct LineMeshProperties
	{
		StrokeCap StartCap;
		StrokeCap EndCap;
		float StrokeWeight;
		uint32_t RoundedCapSegments;
		float Depth;
	};

	struct PointMeshProperties
	{
		float Radius;
		uint32_t Segments;
		float Depth;
	};

	struct TriangleMeshProperties
	{
		float Depth;
	};

	struct QuadMeshProperties
	{
		float Depth;
	};

	struct OutlinedMeshProperties
	{
		float StrokeWeight;
		StrokeJoin JoinStyle;
		float Depth;
	};

	/**
	 * This class is responsible for building meshes from various shapes.
	 *
	 * These shapes are most likely being produced/constructed via the
	 * ShapeBuilder class.
	 */
	class MeshBuilder
	{
	public:

		Mesh GeneratePointMesh(
			const MeshVertex& vertex,
			const PointMeshProperties& properties
		);

		Mesh GenerateLineMesh(
			const std::span<const MeshVertex, 2>& vertices,
			const LineMeshProperties& properties
		);

		Mesh GenerateTriangleMesh(
			const std::span<const MeshVertex, 3>& vertices,
			const TriangleMeshProperties& properties
		);

		Mesh GenerateQuadMesh(
			const std::span<const MeshVertex, 4>& vertices,
			const QuadMeshProperties& properties
		);

		Mesh GenerateOutlinedMesh(
			const std::span<const MeshVertex>& vertices,
			const OutlinedMeshProperties& properties
		);

	};
}