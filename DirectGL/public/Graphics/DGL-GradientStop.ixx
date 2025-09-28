// Project Name : DirectGL
// File Name    : DGL-GradientStop.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:GradientStop;

import :Color;

export namespace DGL
{
	struct GradientStop
	{
		float Position;
		Color Color;
	};
}