// Project Name : DirectGL-Core
// File Name    : DirectGL-ShapeFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module DirectGL:ShapeFactory;

import DirectGL.Renderer;

/*
 * What if there was a DirectGL.Shapes module?
 *
 * It could contain various shape factories, e.g. ShapeFactory2D, ShapeFactory3D, etc.
 * Each factory could provide functions to create different shapes, e.g. FilledRectangle, StrokedCircle, etc.
 * Also, there would be structures that help defining shapes such as BorderSide (Inside, Center, Outside) etc.
 */

namespace DGL
{
	struct ShapeFactory
	{
		Renderer::Vertices FilledRectangle(const Math::FloatBoundary& boundary);
		Renderer::Vertices StrokedRectangle(const Math::FloatBoundary& boundary, float strokeWeight);
	};
}