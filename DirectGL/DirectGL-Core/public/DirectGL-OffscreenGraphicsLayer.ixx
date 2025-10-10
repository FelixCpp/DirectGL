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

import :GraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class OffscreenGraphicsLayer : public GraphicsLayer
	{
	public:

		static std::unique_ptr<OffscreenGraphicsLayer> Create(
			uint32_t width, uint32_t height,
			Renderer::Renderer& renderer
		);

		~OffscreenGraphicsLayer() override;

		void BeginDraw() override;
		void EndDraw() override;

		void PushState() override;
		void PopState() override;
		RenderState& PeekState() override;

		void Fill(Color color) override;
		void Stroke(Color color) override;
		void StrokeWeight(float strokeWeight) override;

		void NoFill() override;
		void NoStroke() override;

		void Blend(const BlendMode& blendMode) override;

		void Background(Color color) override;
		void Rect(float x1, float y1, float x2, float y2) override;
		void Ellipse(float x1, float y1, float x2, float y2) override;
		void Circle(float x1, float y1, float xy2) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;

	private:

		explicit OffscreenGraphicsLayer(
			uint32_t width, uint32_t height,
			Renderer::Renderer& renderer,
			GLuint framebufferId,
			GLuint renderbufferId,
			GLuint renderTextureId
		);

		RenderStateStack m_RenderStates;
		Renderer::Renderer* m_Renderer;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidFillBrush;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidStrokeBrush;
		GLuint m_FramebufferId;
		GLuint m_RenderbufferId;
		GLuint m_RenderTextureId;

		GLint m_CachedFramebufferId;
		Math::UintBoundary m_CachedViewport;

		bool m_IsDrawing;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

	};
}