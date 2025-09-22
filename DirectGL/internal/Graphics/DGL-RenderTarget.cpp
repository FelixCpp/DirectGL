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

	void RenderTarget::FillRectangle(const float left, const float top, const float width, const float height, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledRectangle(left, top, width, height);
		brush.Apply();
		m_Renderer->Submit(geometry);
	}

	void RenderTarget::FillEllipse(const float centerX, const float centerY, const float radiusX, const float radiusY, Brush& brush)
	{
		const Geometry geometry = GeometryFactory::CreateFilledEllipse(centerX, centerY, radiusX, radiusY, 64);
		brush.Apply();
		m_Renderer->Submit(geometry);
	}

}