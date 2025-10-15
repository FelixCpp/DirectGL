// Project Name : DirectGL-Core
// File Name    : DirectGL-DepthProvider.ixx
// Author       : Felix Busch
// Created Date : 2025/10/15

export module DirectGL:DepthProvider;

namespace DGL
{
	struct DepthProvider
	{
		virtual ~DepthProvider() = default;

		virtual float GetDepth() = 0;
		virtual void IncrementDepth() = 0;
		virtual void ResetDepth() = 0;
	};

	class ConstantDepthProvider : public DepthProvider
	{
	public:

		explicit ConstantDepthProvider(float depth);

		float GetDepth() override;
		void IncrementDepth() override;
		void ResetDepth() override;

	private:

		float m_Depth;

	};

	class IncrementalDepthProvider : public DepthProvider
	{
	public:

		explicit IncrementalDepthProvider(float startDepth, float increment);

		float GetDepth() override;
		void IncrementDepth() override;
		void ResetDepth() override;

	private:

		float m_CurrentDepth;
		const float m_StartDepth;
		const float m_Increment;

	};
}
