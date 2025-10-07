// Project Name : DirectGL-Renderer
// File Name    : Renderer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <memory>
#include <type_traits>

export module DirectGL.Renderer;

export import :Color;
export import :SolidColorBrush;
export import :TextureWrapMode;
export import :TextureFilterMode;
export import :VertexRenderer;
export import :Vertices;

import LogForge;

namespace DGL::Renderer
{
	export void SetLogger(std::weak_ptr<LogForge::Logger> logger);
}

module :private;

std::weak_ptr<LogForge::Logger> s_Logger;

namespace DGL::Renderer
{
	void SetLogger(std::weak_ptr<LogForge::Logger> logger)
	{
		s_Logger = std::move(logger);
	}
}
