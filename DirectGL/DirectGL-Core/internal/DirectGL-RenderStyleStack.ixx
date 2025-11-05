// Project Name : DirectGL
// File Name    : DirectGL-RenderStyleStack.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <stack>

export module DirectGL:RenderStyleStack;

import :RenderStyle;

namespace DGL
{
	class RenderStyleStack
	{
	public:

		RenderStyleStack();

		void PushStyle(const RenderStyle& style);
		void PopStyle();
		RenderStyle& PeekStyle();

		void Reset();

	private:

		std::stack<RenderStyle> m_RenderStyles;
		RenderStyle m_DefaultStyle;

	};
}