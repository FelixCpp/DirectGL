// Project Name : DirectGL
// File Name    : DirectGL-DepthProvider.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

export module DirectGL:DepthProvider;

namespace DGL
{
	class DepthProvider
	{
	public:

		explicit DepthProvider(float startDepth = 0.0f, float increment = 0.0001f);

		float GetAndIncrement();
		void Reset();

	private:

		float m_InitialDepth;
		float m_CurrentDepth;
		float m_Increment;

	};
}