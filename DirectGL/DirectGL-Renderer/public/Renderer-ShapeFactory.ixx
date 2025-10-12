// Project Name : DirectGL-Renderer
// File Name    : Renderer-ShapeFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <vcruntime.h>

export module DirectGL.Renderer:ShapeFactory;

import :Vertices;

import DirectGL.Math;

export namespace DGL::Renderer
{
	struct ShapeFactory
	{
		virtual ~ShapeFactory() = default;

		[[nodiscard]] virtual Vertices GetFilledRectangle(const Math::FloatBoundary& boundary) = 0;
		[[nodiscard]] virtual Vertices GetOutlinedRectangle(const Math::FloatBoundary& boundary, float strokeWeight) = 0;

		[[nodiscard]] virtual Vertices GetFilledEllipse(Math::Float2 center, Math::Radius radius, size_t segments) = 0;
		[[nodiscard]] virtual Vertices GetOutlinedEllipse(Math::Float2 center, Math::Radius radius, size_t segments, float strokeWeight) = 0;

		[[nodiscard]] virtual Vertices GetFilledTriangle(Math::Float2 a, Math::Float2 b, Math::Float2 c) = 0;
		//[[nodiscard]] virtual Vertices GetOutlinedTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, float strokeWeight) = 0;

		[[nodiscard]] virtual Vertices GetLine(Math::Float2 start, Math::Float2 end, float strokeWeight) = 0;
	};

	struct DefaultShapeFactory : ShapeFactory
	{
		[[nodiscard]] Vertices GetFilledRectangle(const Math::FloatBoundary& boundary) override;
		[[nodiscard]] Vertices GetOutlinedRectangle(const Math::FloatBoundary& boundary, float strokeWeight) override;

		[[nodiscard]] Vertices GetFilledEllipse(Math::Float2 center, Math::Radius radius, size_t segments) override;
		[[nodiscard]] Vertices GetOutlinedEllipse(Math::Float2 center, Math::Radius radius, size_t segments, float strokeWeight) override;

		[[nodiscard]] Vertices GetFilledTriangle(Math::Float2 a, Math::Float2 b, Math::Float2 c) override;
		//[[nodiscard]] Vertices GetOutlinedTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, float strokeWeight) override;

		[[nodiscard]] Vertices GetLine(Math::Float2 start, Math::Float2 end, float strokeWeight) override;
	};
}