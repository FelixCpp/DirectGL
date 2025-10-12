// Project Name : DirectGL-Core
// File Name    : DirectGL-RenderState.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

module;

export module DirectGL:RenderState;

import :Color;
import :DrawMode;
import :BlendMode;
import :TransformationStack;
import :DrawMode;

export namespace DGL
{
	struct RenderState
	{
		Color FillColor;
		Color StrokeColor;
		float StrokeWeight;

		bool IsFillEnabled;
		bool IsStrokeEnabled;

		BlendMode BlendMode;
		RectMode RectMode;
		EllipseMode EllipseMode;

		TransformationStack TransformationStack;

		RenderState();
	};
}

namespace DGL
{
	RenderState::RenderState():
		FillColor(255, 255, 255),
		StrokeColor(255, 255, 255),
		StrokeWeight(1.0f),
		IsFillEnabled(true),
		IsStrokeEnabled(true),
		BlendMode(BlendModes::Alpha),
		RectMode(RectModeLTWH()),
		EllipseMode(EllipseModeCenterDiameter())
	{
	}
}
