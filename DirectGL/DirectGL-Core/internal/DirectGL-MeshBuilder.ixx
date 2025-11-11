// Project Name : DirectGL
// File Name    : DirectGL-MeshBuilder.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

module;

#include <cstdint>
#include <span>
#include <array>

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

		static Mesh GenerateLineMesh(
			const std::span<const Math::Float2, 2>& points,
			const std::span<const Math::Float4, 2>& colors,
			StrokeCap StrokeCap,
			float strokeWeight,
			uint32_t roundedCapSegments,
			float depth
		);

		static std::vector<Math::Float2> GenerateEllipsePoints(
			Math::Float2 center,
			Math::Radius radius,
			size_t segments
		);

		static Mesh GenerateFilledEllipseMesh(
			Math::Float2 centerPoint,
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float depth
		);

		static std::array<Math::Float2, 4> GenerateQuadPoints(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, const Math::Float2& d);
		static Mesh GenerateFilledQuadMesh(const std::span<const Math::Float2, 4>& points, const std::span<const Math::Float4, 4>& colors, float depth);

		static std::array<Math::Float2, 3> GenerateTrianglePoints(const Math::Float2& v0, const Math::Float2& v1, const Math::Float2& v2);
		static Mesh GenerateFilledTriangleMesh(const std::span<const Math::Float2, 3>& points, const std::span<const Math::Float4, 3>& colors, float depth);

		static Mesh GenerateOutlinedMesh(
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float strokeWeight,
			StrokeJoin strokeJoin,
			uint32_t roundedJoinSegments,
			float depth
		);

	};
}