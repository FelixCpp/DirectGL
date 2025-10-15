module DirectGL;

namespace DGL
{
	
	RendererFacade::RendererFacade(ShapeRenderer::ShapeRenderer& shapeRenderer, ShapeRenderer::ShapeFactory& shapeFactory):
		m_ShapeRenderer(shapeRenderer),
		m_ShapeFactory(shapeFactory)
	{
	}

	void RendererFacade::FillRectangle(const FloatBoundary& boundary, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledRectangle(boundary, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawRectangle(const FloatBoundary& boundary, const float strokeWeight, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedRectangle(boundary, strokeWeight, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillEllipse(const Float2& center, const Radius& radius, const size_t segments, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledEllipse(center, radius, segments, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawEllipse(const Float2& center, const Radius& radius, const size_t segments, const float strokeWeight, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedEllipse(center, radius, segments, strokeWeight, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillTriangle(const Float2& a, const Float2& b, const Float2& c, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetFilledTriangle(a, b, c, depth);
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawLine(const Float2& start, const Float2& end, const float strokeWeight, const float depth)
	{
		const auto vertices = m_ShapeFactory.GetLine(start, end, strokeWeight, depth);
		m_ShapeRenderer.Render(vertices);
	}
}