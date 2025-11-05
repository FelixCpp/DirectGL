// Project Name : DirectGL
// File Name    : DirectGL-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

export module DirectGL:RenderTarget;

namespace DGL
{
	/**
	 * This interface represents a render target that can be activated for rendering.
	 */
	struct RenderTarget
	{
		virtual ~RenderTarget() = default;
		virtual void Activate() = 0;
	};
}