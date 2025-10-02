// Project Name : DirectGL
// File Name    : DGL-OpenGLOffscreenRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

module;

#include <glad/gl.h>

#include <memory>

export module DGL:OpenGLOffscreenRenderTarget;

import :OffscreenRenderTarget;
import :Renderer;
import :Camera;

namespace DGL
{
	class OpenGLOffscreenRenderTarget : public OffscreenRenderTarget
	{
	public:

		static std::unique_ptr<OpenGLOffscreenRenderTarget> Create(uint32_t width, uint32_t height, Renderer& renderer);

		Texture* GetTexture() const override;
		Math::Uint2 GetSize() const override;

		void Begin() override;
		void End() override;

		void DrawRectangle(const Math::FloatBoundary& boundary, float thickness, Brush& brush) override;
		void FillRectangle(const Math::FloatBoundary& boundary, Brush& brush) override;

		void DrawEllipse(const Math::Float2& center, Radius radius, float thickness, Brush& brush) override;
		void FillEllipse(const Math::Float2& center, Radius radius, Brush& brush) override;

		void Line(const Math::Float2& start, const Math::Float2& end, float thickness, Brush& brush, LineCap startCap, LineCap endCap) override;

	private:

		explicit OpenGLOffscreenRenderTarget(
			GLuint framebufferId,
			GLuint renderbufferId,
			std::unique_ptr<Texture> texture,
			Renderer& renderer
		);

		GLuint m_FramebufferId;
		GLuint m_RenderbufferId;

		std::unique_ptr<Texture> m_Texture;

		Renderer* m_Renderer;
		Camera m_Camera;

	};
}