// Project Name : DirectGL
// File Name    : DirectGL-EllipseSegmentsMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <functional>

export module DirectGL:EllipseSegmentsMode;

import DirectGL.Math;

export namespace DGL
{
	typedef std::function<size_t(Math::Radius radius, Math::Angle sweep)> EllipseSegmentsMode;

	const EllipseSegmentsMode& EllipseSegmentsModeFixed(size_t count);
	const EllipseSegmentsMode& EllipseSegmentsModeAdaptive(float tolerance = 0.5f);
}