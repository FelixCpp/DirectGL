// Project Name : DirectGL
// File Name    : DGL-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

export module DGL:RenderTarget;

import :Renderer;
import :Brush;

namespace DGL
{
	class RenderTarget
	{
	public:

		explicit RenderTarget(Renderer& renderer);

		void Clear();
		void FillRectangle(float left, float top, float width, float height, Brush& brush);
		void FillEllipse(float centerX, float centerY, float radiusX, float radiusY, Brush& brush);

	private:

		Renderer* m_Renderer;

	};
}