module DirectGL;

import :RenderStateStack;

namespace DGL
{
	RenderStateStack::RenderStateStack():
		m_CurrentState(&m_DefaultState)
	{
	}

	void RenderStateStack::PushState()
	{
		m_RenderStates.push(*m_CurrentState);
		m_CurrentState = &m_RenderStates.top();
	}

	void RenderStateStack::PopState()
	{
		if (not m_RenderStates.empty())
		{
			m_RenderStates.pop();
			m_CurrentState = m_RenderStates.empty() ? &m_DefaultState : &m_RenderStates.top();
		}
	}

	RenderState& RenderStateStack::PeekState() const
	{
		return *m_CurrentState;
	}
}