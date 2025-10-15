// Project Name : DirectGL-Core
// File Name    : DirectGL-RendererFacade.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

export module DirectGL:RendererFacade;

import DirectGL.Math;
import DirectGL.Renderer;
import DirectGL.ShapeRenderer;

import :DepthProvider;

namespace DGL
{
	/// @brief This class provides a simplified interface to the rendering subsystem.
	///
	/// It encapsulates the functionality of both the Renderer and ShapeRenderer,
	/// PolygonRenderer as well as TextRenderer and provides a unified API for
	/// common rendering tasks.
	class RendererFacade
	{
	public:

		explicit RendererFacade(
			ShapeRenderer::ShapeRenderer& shapeRenderer,
			ShapeRenderer::ShapeFactory& shapeFactory,
			DepthProvider& depthProvider
		);

		void FillRectangle(const Math::FloatBoundary& boundary);
		void DrawRectangle(const Math::FloatBoundary& boundary, float strokeWeight);

		void FillEllipse(const Math::Float2& center, const Math::Radius& radius, size_t segments);
		void DrawEllipse(const Math::Float2& center, const Math::Radius& radius, size_t segments, float strokeWeight);

		void FillTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c);
		void DrawLine(const Math::Float2& start, const Math::Float2& end, float strokeWeight);

	private:

		float IncrementAndGetDepth() const;

		ShapeRenderer::ShapeRenderer& m_ShapeRenderer;
		ShapeRenderer::ShapeFactory& m_ShapeFactory;

		DepthProvider* m_DepthProvider;

	};
}