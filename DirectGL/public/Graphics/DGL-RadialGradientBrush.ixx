// Project Name : DirectGL
// File Name    : DGL-RadialGradientBrush.ixx
// Author       : Felix Busch
// Created Date : 2025/09/22

module;

#include <vector>
#include <array>
#include <memory>

export module DGL:RadialGradientBrush;

import Math;

import :Brush;
import :Color;
import :ShaderProgram;
import :UniformBuffer;

export namespace DGL
{
	struct GradientStop
	{
		float Position;
		Color Color;
	};

	enum class ExtendMode
	{
		Clamp,
		Wrap,
		Mirror,
	};

	enum class GammaMode
	{
		Gamma2_2,
		Gamma1_0,
	};

	class RadialGradientBrush : public Brush
	{
	public:

		struct Properties
		{
			std::vector<GradientStop> GradientStops;
			ExtendMode ExtendMode;
			GammaMode GammaMode;
			Math::Float2 Center;
			Math::Float2 Offset;
			Math::Float2 Radius;
		};

	public:

		static std::unique_ptr<RadialGradientBrush> Create(const Properties& properties);

		void Apply() override;

	private:

		explicit RadialGradientBrush(std::unique_ptr<ShaderProgram> shaderProgram, const Properties& properties);

		std::unique_ptr<ShaderProgram> m_ShaderProgram;
		std::unique_ptr<UniformBuffer> m_UniformBuffer;

		/// This structure represents the 1:1 mirror
		/// of a 140-Std packed GLSL structure
		struct Std140Properties
		{
			Math::Float4 CenterOffset;					//!< Center (2D), Offset (2D)
			Math::Float4 RadiusExtendGamma;				//!< Radius (2D), Extend (1D), Gamma (1D)
			std::array<Math::Float4, 16> Positions;		//!< Positions (1D)
			std::array<Math::Float4, 16> Colors;		//!< Colors (4D)
			int StopCount;								//!< StopCount
		};

		[[nodiscard]] static Std140Properties PropertiesToStd140(const Properties& properties);
		Std140Properties m_Properties;

	};
}