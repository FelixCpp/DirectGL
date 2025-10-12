// Project Name : DirectGL-Core
// File Name    : DirectGL-RenderStateStack.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

#include <stack>

export module DirectGL:RenderStateStack;

import :RenderState;

namespace DGL
{
	class RenderStateStack
	{
	public:

		void PushState();
		void PopState();
		void Clear();

		RenderState& PeekState();

	private:

		std::stack<RenderState> m_RenderStates;
		RenderState m_DefaultState;

	};
}