// Project Name : DirectGL-ShapeRenderer
// File Name    : ShapeRenderer-PrimitiveType.ixx
// Author       : Felix Busch
// Created Date : 2025/10/17

export module DirectGL.ShapeRenderer:PrimitiveType;

export namespace DGL::ShapeRenderer
{
	enum class PrimitiveType
	{
		Points,
		Triangles,
		TriangleStrip,
		TriangleFan,
		Lines,
		LineStrip,
		LineLoop,
	};

	enum class LineCapStyle
	{
		Butt,
		Square,
		Round,
	};

	enum class LineJoinStyle
	{
		Miter,
		Bevel,
		Round,
	};
}