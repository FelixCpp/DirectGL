// Project Name : Math
// File Name    : Math-Remap.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module Math:Remap;

export namespace Math
{
	constexpr float Remap(float value, float istart, float istop, float ostart, float ostop);
	constexpr float Constrain(float value, float min, float max);
	constexpr float Lerp(float start, float end, float t);
}

namespace Math
{
	constexpr float Remap(const float value, const float istart, const float istop, const float ostart, const float ostop)
	{
		return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
	}

	constexpr float Constrain(const float value, const float min, const float max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}

	constexpr float Lerp(const float start, const float end, const float t)
	{
		return start * (1.0f - t) + t * end;
	}
}