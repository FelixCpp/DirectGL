// Project Name : DirectGL
// File Name    : DirectGL-MainRenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/11/05

export module DirectGL:MainRenderTarget;

import :RenderTarget;

import DirectGL.Math;

namespace DGL
{
	class MainRenderTarget : public RenderTarget
	{
	public:

		explicit MainRenderTarget(const Math::FloatBoundary& viewport);

		void SetViewport(const Math::FloatBoundary& viewport);
		const Math::FloatBoundary& GetViewport() const;

		void Activate() override;

	private:

		Math::FloatBoundary m_Viewport;

	};
}