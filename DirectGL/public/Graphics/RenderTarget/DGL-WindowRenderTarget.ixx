// Project Name : DirectGL
// File Name    : DGL-WindowRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:WindowRenderTarget;

import :RenderTarget;
import :Window;

export namespace DGL
{
	struct WindowRenderTarget : RenderTarget
	{
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual Math::Uint2 GetSize() const = 0;
		virtual Window& GetTarget() const = 0;
	};
}