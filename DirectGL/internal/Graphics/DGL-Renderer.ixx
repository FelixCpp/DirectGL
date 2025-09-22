// Project Name : DirectGL
// File Name    : DGL-Renderer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <glad/gl.h>

export module DGL:Renderer;

import :Geometry;

namespace DGL
{
	struct Renderer
	{
		virtual ~Renderer() = default;
		virtual void Submit(const Geometry& geometry) = 0;
	};
}