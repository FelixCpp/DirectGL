module DirectGL;

import :DepthProvider;

namespace DGL
{
	DepthProvider::DepthProvider(const float startDepth, const float increment):
		m_InitialDepth(startDepth),
		m_CurrentDepth(startDepth),
		m_Increment(increment)
	{
	}

	float DepthProvider::GetAndIncrement()
	{
		const float depth = m_CurrentDepth;
		m_CurrentDepth += m_Increment;
		return depth;
	}

	void DepthProvider::Reset()
	{
		m_CurrentDepth = m_InitialDepth;
	}
}