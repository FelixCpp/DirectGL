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

		RenderStateStack();

		void PushState();
		void PopState();
		void Clear();

		RenderState& PeekState() const;

	private:

		std::stack<RenderState> m_RenderStates;
		RenderState m_DefaultState;
		RenderState* m_CurrentState;

	};
}