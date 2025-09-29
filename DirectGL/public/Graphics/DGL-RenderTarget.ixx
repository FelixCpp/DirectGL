// Project Name : DirectGL
// File Name    : DGL-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:RenderTarget;

import :Brush;
import :Camera;

import :BorderRadius;
import :Renderer;
import :Radius;

export namespace DGL
{
	class RenderTarget
	{
	public:

		explicit RenderTarget(Renderer& renderer, Camera* camera);

		void Clear();
		void FillRectangle(const Math::FloatBoundary& boundary, Brush& brush);
		void FillRoundedRectangle(const Math::FloatBoundary& boundary, const BorderRadius& borderRadius, Brush& brush);
		void FillEllipse(const Math::Float2& center, const Radius& radius, Brush& brush);
		void DrawEllipse(const Math::Float2& center, const Radius& radius, float thickness, Brush& brush);

	private:

		Renderer* m_Renderer;
		Camera* m_Camera;

	};
}