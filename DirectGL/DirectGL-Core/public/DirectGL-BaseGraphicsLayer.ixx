// Project Name : DirectGL-Core
// File Name    : DirectGL-BaseGraphicsLayer.ixx
// Author       : Felix Busch
// Created Date : 2025/10/10

module;

#include <memory>

export module DirectGL:BaseGraphicsLayer;

import Math;

import :RenderStateStack;
import :GraphicsLayer;

export namespace DGL
{
	class BaseGraphicsLayer : public GraphicsLayer
	{
	public:

		explicit BaseGraphicsLayer(
			Math::Uint2 viewportSize,
			Renderer::Renderer& renderer,
			Renderer::ShapeFactory& shapeFactory
		);

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
		void Point(float x, float y) override;
		void Line(float x1, float y1, float x2, float y2) override;
		void Triangle(float x1, float y1, float x2, float y2, float x3, float y3) override;

	protected:

		Renderer::Renderer* m_Renderer;
		Renderer::ShapeFactory* m_ShapeFactory;

		std::unique_ptr<Renderer::SolidColorBrush> m_SolidFillBrush;
		std::unique_ptr<Renderer::SolidColorBrush> m_SolidStrokeBrush;

		Math::FloatBoundary m_Viewport;
		Math::Matrix4x4 m_ProjectionMatrix;

		RenderStateStack m_RenderStates;

		bool m_IsDrawing;

	};
}