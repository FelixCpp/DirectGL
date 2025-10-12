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
		while (not m_RenderStates.empty())
			m_RenderStates.pop();
	}

	RenderState& RenderStateStack::PeekState()
	{
		if (m_RenderStates.empty())
			return m_DefaultState;

		return m_RenderStates.top();
	}
}