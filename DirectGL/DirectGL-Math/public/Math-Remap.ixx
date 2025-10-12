// Project Name : Math
// File Name    : Math-Remap.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <algorithm>
#include <cmath>

export module DirectGL.Math:Remap;

export namespace DGL::Math
{
	constexpr float Remap(float value, float istart, float istop, float ostart, float ostop);
	constexpr float Constrain(float value, float min, float max);
	constexpr float Min(float value, float min);
	constexpr float Max(float value, float max);
	constexpr float Lerp(float start, float end, float t);
}

namespace DGL::Math
{
	constexpr float Remap(const float value, const float istart, const float istop, const float ostart, const float ostop)
	{
		return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
	}

	constexpr float Constrain(const float value, const float min, const float max)
	{
		return std::clamp(value, min, max);
	}

	constexpr float Min(const float value, const float min)
	{
		return std::max(value, min);
	}

	constexpr float Max(const float value, const float max)
	{
		return std::min(value, max);
	}

	constexpr float Lerp(const float start, const float end, const float t)
	{
		return std::lerp(start, end, t);
	}
}