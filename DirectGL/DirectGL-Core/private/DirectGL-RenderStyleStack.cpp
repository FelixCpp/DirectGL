module DirectGL;

import :RenderStyleStack;

namespace DGL
{
	RenderStyleStack::RenderStyleStack()
	{
	}

	void RenderStyleStack::PushStyle()
	{
		m_RenderStyles.push(PeekStyle());
	}

	void RenderStyleStack::PopStyle()
	{
		if (not m_RenderStyles.empty())
		{
			m_RenderStyles.pop();
		}
	}

	RenderStyle& RenderStyleStack::PeekStyle()
	{
		if (m_RenderStyles.empty())
		{
			return m_DefaultStyle;
		}

		return m_RenderStyles.top();
	}

	void RenderStyleStack::Reset()
	{
		while (not m_RenderStyles.empty())
		{
			m_RenderStyles.pop();
		}

		m_DefaultStyle = {};
	}

}