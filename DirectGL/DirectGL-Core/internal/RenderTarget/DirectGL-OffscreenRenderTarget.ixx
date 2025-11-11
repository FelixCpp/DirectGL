// Project Name : DirectGL
// File Name    : DirectGL-OffscreenRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <glad/gl.h>

export module DirectGL:OffscreenRenderTarget;

import DirectGL.Math;

import :RenderTarget;

namespace DGL
{
	class OffscreenRenderTarget : public RenderTarget
	{
	public:

		explicit OffscreenRenderTarget(const Math::FloatBoundary& viewport);
		~OffscreenRenderTarget() override;

		void Activate() override;

	private:

		GLuint m_Framebuffer;
		GLuint m_Renderbuffer;
		GLuint m_TextureId;

		Math::FloatBoundary m_Viewport;

	};
}