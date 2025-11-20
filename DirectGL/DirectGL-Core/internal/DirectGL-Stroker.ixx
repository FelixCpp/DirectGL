// Project Name : DirectGL
// File Name    : DirectGL-Stroker.ixx
// Author       : Felix Busch
// Created Date : 2025/11/19

module;

#include <vector>
#include <span>

export module DirectGL:Stroker;

import DirectGL.Math;

import :StrokeCap;
import :StrokeJoin;
import :StrokeAlignment;

namespace DGL
{

	/// @brief This structure represents a single vertex used in stroke generation.
	struct StrokeVertex
	{
		Math::Float2 Position;	//!< The position of the vertex in 2D space.
		Math::Float4 Color;		//!< The color of the vertex.
	};

	/// @brief This structure represents a mesh generated for a stroke.
	///
	/// A mesh consists of vertices and indices that define how those vertices
	/// are connected to form the stroke shape.
	///
	/// We assume that the stroke is represented in 2D space and rendered
	/// as a series of connected triangles.
	struct Stroke
	{
		std::vector<StrokeVertex>	Vertices;	//!< The vertices that make up the stroke mesh.
		std::vector<uint32_t>		Indices;	//!< The indices defining the order of vertices for rendering.
	};

	/// @brief This interface defines the contract for stroke generation algorithms.
	///
	/// It's only purpose is to provide a functional interface for different stroking
	/// implementations.
	struct Stroker
	{
		virtual ~Stroker() = default;

		[[nodiscard]] virtual Stroke GenerateStroke(
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float strokeWeight,
			float miterLimit,
			StrokeAlignment alignment, 
			StrokeCap strokeCap,
			StrokeJoin join,
			bool closed
		) = 0;
	};

	/// @brief This implementation of the Stroker interface generates strokes for polygonal shapes.
	///
	/// Note that this stroker only handles polygons and does not support curves or complex paths.
	/// When stroking shapes with curves, consider tessellating them into polygons first.
	struct PolygonStroker : Stroker
	{
		[[nodiscard]] Stroke GenerateStroke(
			const std::span<const Math::Float2>& points,
			const std::span<const Math::Float4>& colors,
			float strokeWeight,
			float miterLimit,
			StrokeAlignment alignment, 
			StrokeCap strokeCap,
			StrokeJoin join,
			bool isClosed
		) override;
	};
}