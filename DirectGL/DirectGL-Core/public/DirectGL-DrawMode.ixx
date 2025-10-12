// Project Name : DirectGL-Core
// File Name    : DirectGL-DrawMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/12

module;

#include <functional>

export module DirectGL:DrawMode;

import :Math;

export namespace DGL
{
	using RectMode = std::function<FloatBoundary(float, float, float, float)>;

	RectMode RectModeLTWH();
	RectMode RectModeLTRB();
	RectMode RectModeCenterWH();
}

export namespace DGL
{
	using EllipseMode = std::function<FloatBoundary(float, float, float, float)>;

	EllipseMode EllipseModeLTRB();
	EllipseMode EllipseModeLTWH();
	EllipseMode EllipseModeCenterWH();
	EllipseMode EllipseModeCenterRadius();
	EllipseMode EllipseModeCenterDiameter();
}