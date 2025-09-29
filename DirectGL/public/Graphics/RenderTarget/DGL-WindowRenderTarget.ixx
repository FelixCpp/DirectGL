// Project Name : DirectGL
// File Name    : DGL-WindowRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:WindowRenderTarget;

import :RenderTarget;

export namespace DGL
{
	class WindowRenderTarget : public RenderTarget
	{
	public:

		void BeginDraw() override;
		void EndDraw() override;

	};
}