// Project Name : DirectGL-ShapeRenderer
// File Name    : ShapeRenderer-ShapeFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

export module DirectGL.ShapeRenderer:ShapeFactory;

import :Vertices;

export namespace DGL::ShapeRenderer
{
	struct ShapeFactory
	{
		Vertices GetFilledRectangle(const Math::FloatBoundary& boundary, float depth);
		Vertices GetOutlinedRectangle(const Math::FloatBoundary& boundary, float strokeWeight, float depth);
		Vertices GetFilledEllipse(Math::Float2 center, Math::Radius radius, size_t segments, float depth);
		Vertices GetOutlinedEllipse(Math::Float2 center, Math::Radius radius, size_t segments, float strokeWeight, float depth);
		Vertices GetFilledTriangle(Math::Float2 a, Math::Float2 b, Math::Float2 c, float depth);
		Vertices GetLine(Math::Float2 start, Math::Float2 end, float strokeWeight, LineCapStyle startCap, LineCapStyle endCap, float depth);
	};
}