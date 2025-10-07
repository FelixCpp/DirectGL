// Project Name : DirectGL
// File Name    : DGL-LineCap.ixx
// Author       : Felix Busch
// Created Date : 2025/09/30

export module DGL:LineCap;

export namespace DGL
{
	enum class LineCap
	{
		Butt,   //!< The line is squared off at the endpoint.
		Round,  //!< The line is rounded off with a semicircle at the endpoint.
		Square  //!< The line is squared off with a square at the endpoint, extending beyond the endpoint by half the line's thickness.
	};
}