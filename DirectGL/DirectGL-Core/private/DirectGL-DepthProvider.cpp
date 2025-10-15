module DirectGL;

namespace DGL
{
	ConstantDepthProvider::ConstantDepthProvider(const float depth):
		m_Depth(depth)
	{
	}

	float ConstantDepthProvider::GetDepth()
	{
		return m_Depth;
	}

	void ConstantDepthProvider::IncrementDepth()
	{
		// Nothing to do here.
	}

	void ConstantDepthProvider::ResetDepth()
	{
		// Nothing to do here.
	}
}

namespace DGL
{
	IncrementalDepthProvider::IncrementalDepthProvider(const float startDepth, const float increment):
		m_CurrentDepth(0.0f),
		m_StartDepth(startDepth),
		m_Increment(increment)
	{
	}

	float IncrementalDepthProvider::GetDepth()
	{
		return m_CurrentDepth;
	}

	void IncrementalDepthProvider::IncrementDepth()
	{
		m_CurrentDepth += m_Increment;
	}

	void IncrementalDepthProvider::ResetDepth()
	{
		m_CurrentDepth = m_StartDepth;
	}
}