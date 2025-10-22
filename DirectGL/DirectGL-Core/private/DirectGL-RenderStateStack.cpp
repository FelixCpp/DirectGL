module DirectGL;

import :RenderStateStack;

namespace DGL
{
	void RenderStateStack::PushState()
	{
		m_RenderStates.push(PeekState());
	}

	void RenderStateStack::PopState()
	{
		if (not m_RenderStates.empty())
		{
			m_RenderStates.pop();
		}
	}

	void RenderStateStack::Clear()
	{
		// Clear out all render states from the stack
		while (not m_RenderStates.empty())
			m_RenderStates.pop();

		// Reset the default state to its initial values.
		m_DefaultState = {};
	}

	RenderState& RenderStateStack::PeekState()
	{
		if (m_RenderStates.empty())
			return m_DefaultState;

		return m_RenderStates.top();
	}
}