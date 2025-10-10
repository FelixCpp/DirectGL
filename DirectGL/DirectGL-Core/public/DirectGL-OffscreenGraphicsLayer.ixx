// Project Name : DirectGL-Core
// File Name    : DirectGL-OffscreenGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL:OffscreenGraphicsLayer;

import DirectGL.Renderer;
import Math;

import :BaseGraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class OffscreenGraphicsLayer : public BaseGraphicsLayer
	{
	public:

		static std::unique_ptr<OffscreenGraphicsLayer> Create(
			Math::Uint2 viewportSize,
			Renderer::Renderer& renderer,
			Renderer::ShapeFactory& shapeFactory
		);

		~OffscreenGraphicsLayer() override;

		void BeginDraw() override;
		void EndDraw() override;

	private:

		explicit OffscreenGraphicsLayer(
			Math::Uint2 viewportSize,
			Renderer::Renderer& renderer,
			Renderer::ShapeFactory& shapeFactory,
			GLuint framebufferId,
			GLuint renderbufferId,
			GLuint renderTextureId
		);

		GLuint m_FramebufferId;
		GLuint m_RenderbufferId;
		GLuint m_RenderTextureId;

		GLint m_CachedFramebufferId;
		Math::UintBoundary m_CachedViewport;

		bool m_IsDrawing;

	};
}