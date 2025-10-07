// Project Name : DirectGL-RendererAdapter
// File Name    : RendererAdapter-ResourceFactory.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

module;

#include <memory>

export module DirectGL.RendererAdapter:ResourceFactory;

import DirectGL.Renderer;

import :RendererAPI;

namespace DGL::RendererAdapter
{
	struct ResourceFactory
	{
		virtual ~ResourceFactory() = default;
		virtual std::unique_ptr<Renderer::TextureSampler> CreateTextureSampler(Renderer::TextureWrapMode wrapMode, Renderer::TextureFilterMode filterMode) = 0;

		static std::unique_ptr<ResourceFactory> Create(RendererAPI rendererApi);
	};
}