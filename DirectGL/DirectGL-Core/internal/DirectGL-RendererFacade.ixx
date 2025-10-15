// Project Name : DirectGL-Core
// File Name    : DirectGL-RendererFacade.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

export module DirectGL:RendererFacade;

import DirectGL.Renderer;
import DirectGL.ShapeRenderer;

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
			ShapeRenderer::ShapeFactory& shapeFactory
		);

		void FillRectangle(const Math::FloatBoundary& boundary, float depth);
		void DrawRectangle(const Math::FloatBoundary& boundary, float strokeWeight, float depth);

		void FillEllipse(const Math::Float2& center, const Math::Radius& radius, size_t segments, float depth);
		void DrawEllipse(const Math::Float2& center, const Math::Radius& radius, size_t segments, float strokeWeight, float depth);

		void FillTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, float depth);
		void DrawLine(const Math::Float2& start, const Math::Float2& end, float strokeWeight, float depth);

	private:

		ShapeRenderer::ShapeRenderer& m_ShapeRenderer;
		ShapeRenderer::ShapeFactory& m_ShapeFactory;

	};
}