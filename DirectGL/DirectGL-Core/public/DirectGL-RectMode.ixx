// Project Name : DirectGL
// File Name    : DirectGL-RectMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <functional>

export module DirectGL:RectMode;

import DirectGL.Math;

export namespace DGL
{
	typedef std::function<Math::FloatBoundary(float, float, float, float)> RectMode;

	const RectMode& RectModeLTWH();
	const RectMode& RectModeLTRB();
	const RectMode& RectModeCenterWH();
}