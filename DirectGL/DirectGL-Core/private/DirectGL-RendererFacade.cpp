module DirectGL;

namespace DGL
{
	
	RendererFacade::RendererFacade(
		TextureRenderer::TextureRenderer& textureRenderer,
		ShapeRenderer::ShapeRenderer& shapeRenderer,
		ShapeRenderer::ShapeFactory& shapeFactory,
		DepthProvider& depthProvider
	):	m_TextureRenderer(textureRenderer),
		m_ShapeRenderer(shapeRenderer),
		m_ShapeFactory(shapeFactory),
		m_DepthProvider(&depthProvider)
	{
	}

	void RendererFacade::FillRectangle(const Math::FloatBoundary& boundary)
	{
		const auto vertices = m_ShapeFactory.GetFilledRectangle(boundary, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawRectangle(const Math::FloatBoundary& boundary, const float strokeWeight)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedRectangle(boundary, strokeWeight, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillEllipse(const Math::Float2& center, const Math::Radius& radius, const size_t segments)
	{
		const auto vertices = m_ShapeFactory.GetFilledEllipse(center, radius, segments, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::DrawEllipse(const Math::Float2& center, const Math::Radius& radius, const size_t segments, const float strokeWeight)
	{
		const auto vertices = m_ShapeFactory.GetOutlinedEllipse(center, radius, segments, strokeWeight, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::FillTriangle(const Math::Float2& a, const Math::Float2& b, const Math::Float2& c)
	{
		const auto vertices = m_ShapeFactory.GetFilledTriangle(a, b, c, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::Line(const Math::Float2& start, const Math::Float2& end, const float strokeWeight)
	{
		const auto vertices = m_ShapeFactory.GetLine(start, end, strokeWeight, IncrementAndGetDepth());
		m_ShapeRenderer.Render(vertices);
	}

	void RendererFacade::Image(const Math::FloatBoundary& boundary)
	{
		m_TextureRenderer.Render(boundary.Left, boundary.Top, boundary.Width, boundary.Height, IncrementAndGetDepth());
	}

	float RendererFacade::IncrementAndGetDepth() const
	{
		const float currentDepth = m_DepthProvider->GetDepth();
		m_DepthProvider->IncrementDepth();
		return currentDepth;
	}

}