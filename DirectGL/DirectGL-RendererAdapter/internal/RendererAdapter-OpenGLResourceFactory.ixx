// Project Name : DirectGL-RendererAdapter
// File Name    : RendererAdapter-OpenGLResourceFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <memory>

export module DirectGL.RendererAdapter:OpenGLResourceFactory;

import :ResourceFactory;

namespace DGL::RendererAdapter
{
	struct OpenGLResourceFactory : ResourceFactory
	{
		std::unique_ptr<Renderer::TextureSampler> CreateTextureSampler(Renderer::TextureWrapMode wrapMode, Renderer::TextureFilterMode filterMode) override;
	};
}