// Project Name : DirectGL-Core
// File Name    : DirectGL-GraphicsLayerStack.ixx
// Author       : Felix Busch
// Created Date : 2025/10/22

module;

#include <stack>
#include <memory>

export module DirectGL:GraphicsLayerStack;

import :MainGraphicsLayer;
import :OffscreenGraphicsLayer;

namespace DGL
{
	class GraphicsLayerStack
	{
	public:

		explicit GraphicsLayerStack(MainGraphicsLayer* baseLayer);

		void PushLayer(OffscreenGraphicsLayer* layer);
		void PopLayer();

		GraphicsLayer& PeekLayer();

	private:

		MainGraphicsLayer* m_MainLayer;
		std::stack<OffscreenGraphicsLayer*> m_OffscreenLayers;

	};
}