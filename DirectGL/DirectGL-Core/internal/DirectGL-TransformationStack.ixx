// Project Name : DirectGL
// File Name    : DirectGL-MatrixStack.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

module;

#include <stack>

export module DirectGL:MatrixStack;

import DirectGL.Math;

namespace DGL
{
	class MatrixStack
	{
	public:

		void PushMatrix(bool extendCurrentMatrix);
		void PopMatrix();
		Math::Matrix4x4& PeekMatrix();
		const Math::Matrix4x4& PeekMatrix() const;

	private:

		Math::Matrix4x4 m_DefaultMatrix;
		std::stack<Math::Matrix4x4> m_Matrics;

	};
}