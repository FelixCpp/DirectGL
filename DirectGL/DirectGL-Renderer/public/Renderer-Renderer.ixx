// Project Name : DirectGL-Renderer
// File Name    : Renderer-Renderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

export module DirectGL.Renderer:Renderer;

import :Vertices;
import :BlendMode;

export namespace DGL::Renderer
{
	struct Renderer
	{
		virtual ~Renderer() = default;
		virtual void Render(const Vertices& renderer, const BlendMode& blendMode) = 0;
	};
}