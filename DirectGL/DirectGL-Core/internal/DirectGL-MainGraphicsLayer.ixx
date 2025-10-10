// Project Name : DirectGL-Core
// File Name    : DirectGL-MainGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL:MainGraphicsLayer;

import DirectGL.Renderer;
import Math;

import :GraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class MainGraphicsLayer : public GraphicsLayer
	{
	public:

		static std::unique_ptr<MainGraphicsLayer> Create(
			uint32_t width, uint32_t height,
			Renderer::Renderer& renderer
		);

		void Resize(uint32_t width, uint32_t height);

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

		explicit MainGraphicsLayer(uint32_t width, uint32_t height, Renderer::Renderer& renderer);

		RenderStateStack m_RenderStates;
		Renderer::Renderer* m_Renderer;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidFillBrush;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidStrokeBrush;

		GLint m_CachedFramebufferId;
		Math::UintBoundary m_CachedViewport;

		bool m_IsDrawing;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

	};
}