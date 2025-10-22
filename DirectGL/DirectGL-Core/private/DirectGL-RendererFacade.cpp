module;

#include <memory>

module DirectGL;

namespace DGL
{
	
	RendererFacade::RendererFacade(
		TextureRenderer::TextureRenderer& textureRenderer,
		ShapeRenderer::ShapeRenderer& shapeRenderer,
		ShapeRenderer::ShapeFactory& shapeFactory
	):	m_TextureRenderer(textureRenderer),
		m_ShapeRenderer(shapeRenderer),
		m_ShapeFactory(shapeFactory)
	{
	}

	void RendererFacade::FillRectangle(const Math::FloatBoundary& boundary, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledRectangle(boundary, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawRectangle(const Math::FloatBoundary& boundary, const float strokeWeight, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedRectangle(boundary, strokeWeight, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillEllipse(const Math::Float2& center, const Math::Radius& radius, const size_t segments, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledEllipse(center, radius, segments, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawEllipse(const Math::Float2& center, const Math::Radius& radius, const size_t segments, const float strokeWeight, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedEllipse(center, radius, segments, strokeWeight, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledTriangle(a, b, c, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::Line(const Math::Float2& start, const Math::Float2& end, const float strokeWeight, const ShapeRenderer::LineCapStyle startCap, const ShapeRenderer::LineCapStyle endCap, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetLine(start, end, strokeWeight, startCap, endCap, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::Image(const Math::FloatBoundary& boundary, const float depth)
	{
		m_TextureRenderer.Render(boundary.Left, boundary.Top, boundary.Width, boundary.Height, depth);
	}

}