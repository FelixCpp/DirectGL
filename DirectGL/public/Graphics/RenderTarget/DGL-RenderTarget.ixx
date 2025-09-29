// Project Name : DirectGL
// File Name    : DGL-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:RenderTarget;

export namespace DGL
{
	struct RenderTarget
	{
		virtual ~RenderTarget() = default;

		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
	};
}