module DirectGL;

import :TransformationStack;

namespace DGL
{
	void TransformationStack::PushTransform()
	{
		m_Transforms.push(PeekTransform());
	}

	void TransformationStack::PopTransform()
	{
		if (not m_Transforms.empty())
		{
			m_Transforms.pop();
		}
	}

	void TransformationStack::Clear()
	{
		while (not m_Transforms.empty())
			m_Transforms.pop();
	}

	Math::Matrix4x4& TransformationStack::PeekTransform()
	{
		if (m_Transforms.empty())
			return m_DefaultTransform;

		return m_Transforms.top();
	}
}