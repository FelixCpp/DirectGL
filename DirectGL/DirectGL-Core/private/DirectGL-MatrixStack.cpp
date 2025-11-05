module DirectGL;

import :MatrixStack;

namespace DGL
{
	void MatrixStack::PushMatrix(const bool extendCurrentMatrix)
	{
		m_Matrics.emplace(extendCurrentMatrix ? PeekMatrix() : Math::Matrix4x4::Identity);
	}

	void MatrixStack::PopMatrix()
	{
		if (not m_Matrics.empty())
		{
			m_Matrics.pop();
		}
	}

	Math::Matrix4x4& MatrixStack::PeekMatrix()
	{
		if (m_Matrics.empty())
		{
			return m_DefaultMatrix;
		}

		return m_Matrics.top();
	}

	const Math::Matrix4x4& MatrixStack::PeekMatrix() const
	{
		if (m_Matrics.empty())
		{
			return m_DefaultMatrix;
		}

		return m_Matrics.top();
	}
}
