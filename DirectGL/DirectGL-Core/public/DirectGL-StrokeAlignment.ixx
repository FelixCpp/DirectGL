// Project Name : DirectGL
// File Name    : DirectGL-StrokeAlignment.ixx
// Author       : Felix Busch
// Created Date : 2025/11/19

export module DirectGL:StrokeAlignment;

export namespace DGL
{
	/// @brief Stroke alignment options.
	enum class StrokeAlignment
	{
		Outside,	//!< The stroke lies outside the shape's geometry.
		Center,		//!< The stroke is centered on the shape's geometry.
		Inside,		//!< The stroke lies inside the shape's geometry.
	};
}