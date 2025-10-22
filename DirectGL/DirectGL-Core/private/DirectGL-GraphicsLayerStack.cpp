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
		// Suspend the current layer
		if (not m_OffscreenLayers.empty())
		{
			m_OffscreenLayers.top()->Suspend();
		}
		else
		{
			m_MainLayer->Suspend();
		}

		// Push & Activate
		m_OffscreenLayers.push(layer);
		m_OffscreenLayers.top()->BeginDraw();
	}

	void GraphicsLayerStack::PopLayer()
	{
		if (not m_OffscreenLayers.empty())
		{
			// Deactivate & Pop
			m_OffscreenLayers.top()->EndDraw();
			m_OffscreenLayers.pop();
		}

		// Resume the previous layer
		if (not m_OffscreenLayers.empty())
		{
			m_OffscreenLayers.top()->Resume();
		}
		else
		{
			m_MainLayer->Resume();
		}
	}

	GraphicsLayer& GraphicsLayerStack::PeekLayer()
	{
		if (m_OffscreenLayers.empty())
		{
			return *m_MainLayer;
		}

		return *m_OffscreenLayers.top();
	}
}