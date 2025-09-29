// Project Name : DirectGL
// File Name    : DGL-FramebufferRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

module;

#include <memory>

export module DGL:FramebufferRenderTarget;

import :RenderTarget;
import :Framebuffer;
import :Renderer;

export namespace DGL
{
	class FramebufferRenderTarget : public RenderTarget
	{
	public:

		explicit FramebufferRenderTarget(
			std::unique_ptr<Framebuffer> framebuffer,
			Renderer* renderer
		);

		void BeginDraw() override;
		void EndDraw() override;

	private:

		std::unique_ptr<Framebuffer> m_Framebuffer;
		Renderer* m_Renderer;

	};
}
