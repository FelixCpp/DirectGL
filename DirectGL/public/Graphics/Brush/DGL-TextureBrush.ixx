// Project Name : DirectGL
// File Name    : DGL-TextureBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:TextureBrush;

import :Brush;
import :Texture;
import :TextureSampler;

import Math;

export namespace DGL
{
	struct TextureBrush : Brush
	{
		virtual void SetTexture(const Texture* texture) = 0;
		virtual const Texture* GetTexture() const = 0;

		virtual void SetTextureSampler(const TextureSampler* textureSampler) = 0;
		virtual const TextureSampler* GetTextureSampler() const = 0;

		virtual void SetSourceRect(const Math::FloatBoundary& boundary) = 0;
		virtual const Math::FloatBoundary& GetSourceRect() const = 0;
	};
}