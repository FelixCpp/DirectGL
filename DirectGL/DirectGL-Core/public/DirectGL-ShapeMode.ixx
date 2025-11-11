// Project Name : DirectGL
// File Name    : DirectGL-ShapeMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

export module DirectGL:ShapeMode;

export namespace DGL
{
	enum class ShapeMode
	{
		Points,
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		TriangleFan,
		Quads,
		QuadStrip,
	};

	enum class ShapeClosingMode
	{
		Open,
		Closed,
	};
}