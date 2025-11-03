// Project Name : DirectGL
// File Name    : DirectGL-QuadRenderer.ixx
// Author       : Felix Busch
// Created Date : 2025/11/03

module;

#include <array>

export module DirectGL:QuadRenderer;

import DirectGL.Math;

import :Color;

namespace DGL
{
	struct QuadRenderer
	{
		struct QuadVertex
		{
			Math::Float3	WorldPosition;
			Math::Float2	LocalPosition;
			Math::Float4	FillColor;
			Math::Float4	StrokeColor;
			Math::Float2	Size;
			float			StrokeWeight;
			float			BorderRadius;
		};

		struct QuadProperties
		{
			std::array<QuadVertex, 4>	Vertices;
		};

		virtual ~QuadRenderer() = default;
		virtual void BeginDraw(const Math::Matrix4x4& projectionViewMatrix) = 0;
		virtual void EndDraw() = 0;
		virtual void SubmitQuad(const QuadProperties& properties) = 0;
	};
}