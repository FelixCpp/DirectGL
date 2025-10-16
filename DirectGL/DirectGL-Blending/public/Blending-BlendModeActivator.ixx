// Project Name : DirectGL-Blending
// File Name    : Blending-BlendModeActivator.ixx
// Author       : Felix Busch
// Created Date : 2025/10/16

export module DirectGL.Blending:BlendModeActivator;

import :BlendMode;

export namespace DGL::Blending
{
	struct BlendModeActivator
	{
		virtual ~BlendModeActivator() = default;
		virtual void Activate(const BlendMode& blendMode) = 0;
	};

	class DefaultBlendModeActivator : public BlendModeActivator
	{
	public:

		void Activate(const BlendMode& blendMode) override;

	};

	class CachingBlendModeActivator : public BlendModeActivator
	{
	public:

		explicit CachingBlendModeActivator(BlendModeActivator& activator);
		void Activate(const BlendMode& blendMode) override;

	private:

		BlendModeActivator& m_Activator;
		BlendMode m_CurrentBlendMode;
		bool m_IsFirstActivation = true;
	};
}