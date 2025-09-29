// Project Name : DirectGL
// File Name    : DGL-RenderTarget.ixx
// Author       : Felix Busch
// Created Date : 2025/09/29

export module DGL:RenderTarget;

import Math;

import :Brush;
import :Radius;

export namespace DGL
{
	struct RenderTarget
	{
		virtual ~RenderTarget() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		virtual void DrawRectangle(const Math::FloatBoundary& boundary, float thickness, Brush& brush) = 0;
		virtual void FillRectangle(const Math::FloatBoundary& boundary, Brush& brush) = 0;

		virtual void DrawEllipse(const Math::Float2& center, Radius radius, float thickness, Brush& brush) = 0;
		virtual void FillEllipse(const Math::Float2& center, Radius radius, Brush& brush) = 0;
	};
}