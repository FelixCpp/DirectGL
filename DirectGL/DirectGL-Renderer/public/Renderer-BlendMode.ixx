// Project Name : DirectGL-Renderer
// File Name    : Renderer-BlendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module DirectGL.Renderer:BlendMode;

export namespace DGL::Renderer
{
	struct BlendMode
	{
		enum class Factor
		{
			Zero,
			One,
			SrcColor,
			OneMinusSrcColor,
			DstColor,
			OneMinusDstColor,
			SrcAlpha,
			OneMinusSrcAlpha,
			DstAlpha,
			OneMinusDstAlpha,
			ConstantColor,
			OneMinusConstantColor,
			ConstantAlpha,
			OneMinusConstantAlpha,
			SrcAlphaSaturate
		};

		enum class Equation
		{
			Add,
			Subtract,
			ReverseSubtract,
			Min,
			Max
		};

		constexpr BlendMode();
		constexpr BlendMode(Factor srcFactor, Factor dstFactor, Equation blendEq);
		constexpr BlendMode(Factor srcFactorRGB, Factor dstFactorRGB, Equation blendEqRGB, Factor srcFactorAlpha, Factor dstFactorAlpha, Equation blendEqAlpha);

		constexpr bool operator == (const BlendMode& other) const;
		constexpr bool operator != (const BlendMode& other) const;

		Factor SourceFactorRGB;			//!< The source factor for RGB channels
		Factor DestinationFactorRGB;	//!< The destination factor for RGB channels
		Equation BlendEquationRGB;		//!< The blend equation for RGB channels
		Factor SourceFactorAlpha;		//!< The source factor for Alpha channel
		Factor DestinationFactorAlpha;	//!< The destination factor for Alpha channel
		Equation BlendEquationAlpha;	//!< The blend equation for Alpha channel
	};
}

namespace DGL::Renderer
{
	constexpr BlendMode::BlendMode()
		: SourceFactorRGB(Factor::One)
		, DestinationFactorRGB(Factor::Zero)
		, BlendEquationRGB(Equation::Add)
		, SourceFactorAlpha(Factor::One)
		, DestinationFactorAlpha(Factor::Zero)
		, BlendEquationAlpha(Equation::Add)
	{
	}

	constexpr BlendMode::BlendMode(const Factor srcFactor, const Factor dstFactor, const Equation blendEq)
		: SourceFactorRGB(srcFactor)
		, DestinationFactorRGB(dstFactor)
		, BlendEquationRGB(blendEq)
		, SourceFactorAlpha(srcFactor)
		, DestinationFactorAlpha(dstFactor)
		, BlendEquationAlpha(blendEq)
	{
	}

	constexpr BlendMode::BlendMode(const Factor srcFactorRGB, const Factor dstFactorRGB, const Equation blendEqRGB, const Factor srcFactorAlpha, const Factor dstFactorAlpha, const Equation blendEqAlpha)
		: SourceFactorRGB(srcFactorRGB)
		, DestinationFactorRGB(dstFactorRGB)
		, BlendEquationRGB(blendEqRGB)
		, SourceFactorAlpha(srcFactorAlpha)
		, DestinationFactorAlpha(dstFactorAlpha)
		, BlendEquationAlpha(blendEqAlpha)
	{
	}

	constexpr bool BlendMode::operator==(const BlendMode& other) const
	{
		return (SourceFactorRGB == other.SourceFactorRGB)
			and (DestinationFactorRGB == other.DestinationFactorRGB)
			and (BlendEquationRGB == other.BlendEquationRGB)
			and (SourceFactorAlpha == other.SourceFactorAlpha)
			and (DestinationFactorAlpha == other.DestinationFactorAlpha)
			and (BlendEquationAlpha == other.BlendEquationAlpha);
	}

	constexpr bool BlendMode::operator!=(const BlendMode& other) const
	{
		return !(*this == other);
	}
}