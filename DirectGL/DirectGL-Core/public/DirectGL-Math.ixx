// Project Name : DirectGL-Core
// File Name    : DirectGL-Math.ixx
// Author       : Felix Busch
// Created Date : 2025/10/12

export module DirectGL:Math;

import DirectGL.Math;

/// @brief This namespace wraps all math related functionality of the DirectGL-Math library
///		   and re-exports it into the DirectGL namespace for easier access.
///
///	This pattern allows us to give fine-grained control over what parts of the
/// DirectGL-Math library are exposed to the users of the DirectGL library,
/// while still keeping the implementation details hidden.
export namespace DGL
{
	// Types
	using Math::Angle; using Math::Degrees; using Math::Radians;
	using Math::Radius, Math::BorderRadius, Math::BorderRadiusOnly;
	using Math::Boundary, Math::FloatBoundary, Math::IntBoundary, Math::UintBoundary;
	using Math::Matrix4x4;
	using Math::Value2, Math::Float2, Math::Int2, Math::Uint2;
	using Math::Value4, Math::Float4, Math::Int4, Math::Uint4;

	// Constants
	using Math::PI;
	using Math::TAU;

	// Utility functions
	using Math::Remap;
	using Math::Constrain;
	using Math::Min;
	using Math::Max;
	using Math::Lerp;
	using Math::Random;

	// Easing functions
	using Math::Easings::EaseInSine; using Math::Easings::EaseOutSine; using Math::Easings::EaseInOutSine;
	using Math::Easings::EaseInCubic; using Math::Easings::EaseOutCubic; using Math::Easings::EaseInOutCubic;
	using Math::Easings::EaseInQuint; using Math::Easings::EaseOutQuint; using Math::Easings::EaseInOutQuint;
	using Math::Easings::EaseInCirc; using Math::Easings::EaseOutCirc; using Math::Easings::EaseInOutCirc;
	using Math::Easings::EaseInElastic; using Math::Easings::EaseOutElastic; using Math::Easings::EaseInOutElastic;
	using Math::Easings::EaseInQuad; using Math::Easings::EaseOutQuad; using Math::Easings::EaseInOutQuad;
	using Math::Easings::EaseInQuart; using Math::Easings::EaseOutQuart; using Math::Easings::EaseInOutQuart;
	using Math::Easings::EaseInExpo; using Math::Easings::EaseOutExpo; using Math::Easings::EaseInOutExpo;
	using Math::Easings::EaseInBack; using Math::Easings::EaseOutBack; using Math::Easings::EaseInOutBack;
	using Math::Easings::EaseInBounce; using Math::Easings::EaseOutBounce; using Math::Easings::EaseInOutBounce;
}