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
	/**
	 * This class is responsible for building meshes from various shapes.
	 *
	 * These shapes are most likely being produced/constructed via the
	 * ShapeBuilder class.
	 */
	class MeshBuilder
	{
	public:

		static Mesh GeneratePointMesh(
			const Math::Float2& center,
			const Math::Float4& color,
			float radius,
			uint32_t segments,
			float depth
		);

		static Mesh GenerateLineMesh(
			const std::span<const Math::Float2, 2>& points,
			const std::span<const Math::Float4, 2>& colors,
			StrokeCap startCap,
			StrokeCap endCap,
			float strokeWeight,
			uint32_t roundedCapSegments,
			float depth
		);

		static Mesh GenerateTriangleMesh(
			const std::span<const Math::Float2, 3>& points,
			const std::span<const Math::Float4, 3>& colors,
			float depth
		);

		static Mesh GenerateQuadMesh(
			const std::span<const Math::Float2, 4>& points,
			const std::span<const Math::Float4, 4>& colors,
			float depth
		);

		static Mesh GenerateOutlinedMesh(
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float strokeWeight,
			StrokeJoin joinStyle,
			StrokeCap startCap,
			StrokeCap endCap,
			bool closed,
			float depth
		);

	};
}