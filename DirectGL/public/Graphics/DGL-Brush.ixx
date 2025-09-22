// Project Name : DirectGL
// File Name    : DGL-Brush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:Brush;

export namespace DGL
{
	struct Brush
	{
		virtual ~Brush() = default;
		virtual void Apply() = 0;
	};
}