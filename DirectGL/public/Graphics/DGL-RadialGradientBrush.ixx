// Project Name : DirectGL
// File Name    : DGL-RadialGradientBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <vector>
#include <array>

export module DGL:RadialGradientBrush;

import Math;

import :Brush;
import :ShaderProgram;
import :UniformBuffer;

export namespace DGL
{
	struct GradientStop
	{
		float Progress;
		float Red, Green, Blue, Alpha;
	};

	enum class ExtendMode
	{
		Clamp,
		Wrap,
		Mirror,
	};

	enum class Gamma
	{
		Gamma2_2,
		Gamma1_0,
	};

	class RadialGradientBrush : public Brush
	{
	public:

		explicit RadialGradientBrush(
			const std::vector<GradientStop>& stops,
			ExtendMode extendMode,
			Gamma gamma,
			const Math::Float2& center,
			const Math::Float2& offset,
			const Math::Float2& radius
		);

		void Apply() override;

	private:

		ShaderProgram m_ShaderProgram;
		std::array<float, 16> m_StopProgress;
		std::array<float, 16 * 4> m_StopColor;
		std::uint32_t m_StopCount;

		ExtendMode m_ExtendMode;
		Gamma m_Gamma;
		Math::Float2 m_Center;
		Math::Float2 m_Offset;
		Math::Float2 m_Radius;

	};
}