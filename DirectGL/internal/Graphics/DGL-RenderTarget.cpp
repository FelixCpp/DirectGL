module DGL;

namespace DGL
{
	RenderTarget::RenderTarget(Renderer& renderer):
		m_Renderer(&renderer)
	{
	}

	void RenderTarget::Clear()
	{

	}

	void RenderTarget::FillRectangle(const Math::FloatBoundary& boundary, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledRectangle(boundary);
		brush.Apply();
		m_Renderer->Submit(geometry);
	}

	void RenderTarget::FillRoundedRectangle(const Math::FloatBoundary& boundary, const BorderRadius& borderRadius, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledRoundedRectangle(boundary, borderRadius, 16, 16, 16, 16);
		brush.Apply();
		m_Renderer->Submit(geometry);
	}

	void RenderTarget::FillEllipse(const Math::Float2& center, const Radius& radius, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledEllipse(center, radius, 64);
		brush.Apply();
		m_Renderer->Submit(geometry);
	}

}