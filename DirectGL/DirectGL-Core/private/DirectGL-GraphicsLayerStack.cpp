module DirectGL;

import :GraphicsLayerStack;

namespace DGL
{
	GraphicsLayerStack::GraphicsLayerStack(MainGraphicsLayer* baseLayer) :
		m_MainLayer(baseLayer)
	{
	}

	void GraphicsLayerStack::PushLayer(OffscreenGraphicsLayer* layer)
	{
		m_OffscreenLayers.push(layer);
		layer->BeginDraw();
	}

	void GraphicsLayerStack::PopLayer()
	{
		if (not m_OffscreenLayers.empty())
		{
			m_OffscreenLayers.top()->EndDraw();
			m_OffscreenLayers.pop();

			if (m_OffscreenLayers.empty())
			{
				m_MainLayer->BeginDraw();
			}
			else
			{
				m_OffscreenLayers.top()->BeginDraw();
			}
		}
	}

	GraphicsLayer& GraphicsLayerStack::GetCurrentLayer()
	{
		if (m_OffscreenLayers.empty())
		{
			return *m_MainLayer;
		}
		else
		{
			return *(m_OffscreenLayers.top());
		}
	}
}