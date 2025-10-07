// Project Name : DirectGL-Renderer
// File Name    : Renderer-TextureSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Renderer:TextureSampler;

import :TextureWrapMode;
import :TextureFilterMode;

export namespace DGL::Renderer
{
	struct TextureSampler
	{
		virtual ~TextureSampler() = default;

		virtual void SetWrapMode(TextureWrapMode mode) = 0;
		virtual TextureWrapMode GetWrapMode() const = 0;

		virtual void SetFilterMode(TextureFilterMode mode) = 0;
		virtual TextureFilterMode GetFilterMode() const = 0;
	};
}