// Project Name : DirectGL-Renderer
// File Name    : Renderer-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/10/12

module;

#include <glad/gl.h>

#include <memory>
#include <optional>

export module DirectGL.Renderer:RenderTarget;

import DirectGL.Math;
import DirectGL.Texture;

export namespace DGL::Renderer
{
	/// The RenderTarget interface represents a target for rendering operations.
	/// Every call between BeginDraw() and EndDraw() will render to this target
	/// unless another RenderTarget is bound in between.
	struct RenderTarget
	{
		virtual ~RenderTarget() = default;

		/// Activates this render target for drawing.
		virtual void Activate() = 0;
	};
}

export namespace DGL::Renderer
{
	/// This implementation renders to the main framebuffer (i.e., the screen).
	class MainRenderTarget : public RenderTarget
	{
	public:

		static std::unique_ptr<MainRenderTarget> Create(Math::UintBoundary viewport);

		void SetViewport(Math::UintBoundary viewport);
		const Math::UintBoundary& GetViewport() const;

		void Activate() override;

	private:

		explicit MainRenderTarget(Math::UintBoundary viewport);

		Math::UintBoundary m_Viewport;

	};
}

export namespace DGL::Renderer
{
	/// This implementation renders to an offscreen framebuffer.
	class OffscreenRenderTarget : public RenderTarget
	{
	public:

		static std::unique_ptr<OffscreenRenderTarget> Create(Math::Uint2 viewportSize);

		~OffscreenRenderTarget() override;

		void Activate() override;
		const Texture::Texture& GetRenderTexture() const;

	private:

		explicit OffscreenRenderTarget(
			Math::Uint2 viewportSize,
			GLuint framebufferId,
			GLuint renderbufferId,
			std::unique_ptr<Texture::Texture> renderTexture
		);

		GLuint m_FramebufferId;
		GLuint m_RenderbufferId;
		std::unique_ptr<Texture::Texture> m_RenderTexture;

		Math::Uint2 m_ViewportSize;

	};
}