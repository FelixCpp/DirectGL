// Project Name : DirectGL
// File Name    : DGL-OffscreenRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:OffscreenRenderTarget;

import :RenderTarget;
import :Texture;

import Math;

export namespace DGL
{
	struct OffscreenRenderTarget : RenderTarget
	{
		virtual Texture* GetTexture() const = 0;
		virtual Math::Uint2 GetSize() const = 0;
	};
}