module DirectGL;

import :TransformationStack;

namespace DGL
{
	void TransformationStack::PushTransform()
	{
		m_Transforms.push(*m_CurrentTransform);
		m_CurrentTransform = &m_Transforms.top();
	}

	void TransformationStack::PopTransform()
	{
		if (not m_Transforms.empty())
		{
			m_Transforms.pop();
			m_CurrentTransform = m_Transforms.empty() ? &m_DefaultTransform : &m_Transforms.top();
		}
	}

	Math::Matrix4x4& TransformationStack::PeekTransform() const
	{
		return *m_CurrentTransform;
	}

	void TransformationStack::Clear()
	{
		while (not m_Transforms.empty())
			m_Transforms.pop();
	}
}