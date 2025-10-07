// Project Name : DirectGL
// File Name    : DGL-Renderer.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:Renderer;

import :Geometry;
import :ShaderProgram;
import :Texture;
import :TextureSampler;

export namespace DGL
{
	struct Renderer
	{
		virtual ~Renderer() = default;
		virtual void Submit(const Geometry& geometry) = 0;
	};
}