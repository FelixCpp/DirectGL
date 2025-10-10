// Project Name : DirectGL-Core
// File Name    : TransformationStack.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <stack>

export module DirectGL:TransformationStack;

import Math;

namespace DGL
{
	class TransformationStack
	{
	public:

		void PushTransform();
		void PopTransform();
		Math::Matrix4x4& PeekTransform() const;

		void Clear();

	private:

		std::stack<Math::Matrix4x4> m_Transforms;
		Math::Matrix4x4 m_DefaultTransform;
		Math::Matrix4x4* m_CurrentTransform = &m_DefaultTransform;

	};
}
