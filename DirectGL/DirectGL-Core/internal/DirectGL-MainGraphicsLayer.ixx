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

import :BaseGraphicsLayer;
import :RenderStateStack;

export namespace DGL
{
	class MainGraphicsLayer : public BaseGraphicsLayer
	{
	public:

		static std::unique_ptr<MainGraphicsLayer> Create(
			Math::Uint2 viewportSize,
			Renderer::Renderer& renderer,
			Renderer::ShapeFactory& shapeFactory
		);

		void Resize(uint32_t width, uint32_t height);

		void BeginDraw() override;
		void EndDraw() override;

	private:

		explicit MainGraphicsLayer(
			Math::Uint2 viewportSize,
			Renderer::Renderer& renderer,
			Renderer::ShapeFactory& shapeFactory
		);

		GLint m_CachedFramebufferId;
		Math::UintBoundary m_CachedViewport;

		bool m_IsDrawing;

	};
}