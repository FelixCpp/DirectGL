// Project Name : DirectGL-Renderer
// File Name    : Renderer-Shapes.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <cstdint>

export module DirectGL.Renderer:Shapes;

import Math;

import :Vertices;

export namespace DGL::Renderer
{
	Vertices GetFilledRectangle(const Math::FloatBoundary& boundary);
	Vertices GetOutlinedRectangle(const Math::FloatBoundary& boundary, float strokeWeight);

	Vertices GetFilledEllipse(Math::Float2 center, Math::Radius radius, uint32_t segments);
	Vertices GetOutlinedEllipse(Math::Float2 center, Math::Radius radius, float strokeWeight, uint32_t segments);

	Vertices GetLine(Math::Float2 start, Math::Float2 end, float strokeWeight);

	Vertices GetFilledTriangle(Math::Float2 v1, Math::Float2 v2, Math::Float2 v3);
}