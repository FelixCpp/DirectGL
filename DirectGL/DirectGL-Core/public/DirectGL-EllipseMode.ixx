// Project Name : DirectGL
// File Name    : DirectGL-EllipseMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <functional>

export module DirectGL:EllipseMode;

import DirectGL.Math;

namespace DGL
{
	typedef std::function<Math::FloatBoundary(float x1, float y1, float x2, float y2)> EllipseMode;

	const EllipseMode& EllipseModeLTWH();
	const EllipseMode& EllipseModeLTRB();
	const EllipseMode& EllipseModeCenterRadius();
	const EllipseMode& EllipseModeCenterDiameter();
}