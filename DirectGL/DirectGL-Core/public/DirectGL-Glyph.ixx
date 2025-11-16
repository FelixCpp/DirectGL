// Project Name : DirectGL
// File Name    : DirectGL-Glyph.ixx
// Author       : Felix Busch
// Created Date : 2025/11/16

module;

#include <cstdint>

export module DirectGL:Glyph;

import DirectGL.Math;

export namespace DGL
{
	struct Glyph
	{
		size_t				AtlasIndex;		//!< Index of the texture atlas where the glyph is stored (most likely used internally)
		Math::UintBoundary	AtlasRect;		//!< Rectangle in the texture atlas where the glyph image is stored (in pixels)
		Math::FloatBoundary	AtlasRectUV;	//!< Rectangle in the texture atlas where the glyph image is stored (in UV coordinates)
		Math::Int2			Bearing;		//!< Offset from the baseline to the left/top of the glyph (in pixels)
		uint32_t			Advance;		//!< Horizontal offset to advance to the next glyph (in pixels)
	};
}