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

	const RectMode& RectModeLTWH();
	const RectMode& RectModeLTRB();
	const RectMode& RectModeCenterWH();
}

export namespace DGL
{
	using EllipseMode = std::function<FloatBoundary(float, float, float, float)>;

	const EllipseMode& EllipseModeLTRB();
	const EllipseMode& EllipseModeLTWH();
	const EllipseMode& EllipseModeCenterWH();
	const EllipseMode& EllipseModeCenterRadius();
	const EllipseMode& EllipseModeCenterDiameter();
}

export namespace DGL
{
	using SegmentCountMode = std::function<size_t(Radius)>;

	const SegmentCountMode& SegmentCountModeFixed(size_t count);
	const SegmentCountMode& SegmentCountModeSmooth(float error = 0.5f);
}