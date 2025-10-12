// Project Name : DirectGL-Core
// File Name    : TransformationStack.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <stack>

export module DirectGL:TransformationStack;

import DirectGL.Math;

namespace DGL
{
	class TransformationStack
	{
	public:

		void PushTransform();
		void PopTransform();
		void Clear();

		Math::Matrix4x4& PeekTransform();

	private:

		std::stack<Math::Matrix4x4> m_Transforms;
		Math::Matrix4x4 m_DefaultTransform;

	};
}
