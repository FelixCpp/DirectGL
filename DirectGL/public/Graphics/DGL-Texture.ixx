// Project Name : DirectGL
// File Name    : DGL-Texture.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <cstdint>

export module DGL:Texture;

import Math;

export namespace DGL
{
	struct Texture
	{
		virtual ~Texture() = default;

		virtual Math::Uint2 GetSize() const = 0;
	};
}