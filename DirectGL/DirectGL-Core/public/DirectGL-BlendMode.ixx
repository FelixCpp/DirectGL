// Project Name : DirectGL
// File Name    : DirectGL-BlendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/10

export module DirectGL:BlendMode;

export namespace DGL
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

		constexpr BlendMode(
			Factor sourceColorFactor, Factor destinationColorFactor, Equation colorEquation,
			Factor sourceAlphaFactor, Factor destinationAlphaFactor, Equation alphaEquation
		);

		constexpr bool operator == (const BlendMode& other) const = default;
		constexpr bool operator != (const BlendMode& other) const = default;

		static const BlendMode Alpha;
		static const BlendMode Multiply;
		static const BlendMode Additive;
		static const BlendMode Opaque;
		static const BlendMode Invert;

		Factor SourceColorFactor;
		Factor DestinationColorFactor;
		Equation ColorEquation;

		Factor SourceAlphaFactor;
		Factor DestinationAlphaFactor;
		Equation AlphaEquation;
	};
}

namespace DGL
{
	constexpr BlendMode::BlendMode(
		const Factor sourceColorFactor, const Factor destinationColorFactor, const Equation colorEquation,
		const Factor sourceAlphaFactor, const Factor destinationAlphaFactor, const Equation alphaEquation
	) :	SourceColorFactor(sourceColorFactor),
		DestinationColorFactor(destinationColorFactor),
		ColorEquation(colorEquation),
		SourceAlphaFactor(sourceAlphaFactor),
		DestinationAlphaFactor(destinationAlphaFactor),
		AlphaEquation(alphaEquation)
	{
	}

	inline constexpr BlendMode BlendMode::Alpha = BlendMode(
		Factor::SrcAlpha, Factor::OneMinusSrcAlpha, Equation::Add,
		Factor::One, Factor::OneMinusSrcAlpha, Equation::Add
	);

	inline constexpr BlendMode BlendMode::Multiply = BlendMode(
		Factor::DstColor, Factor::Zero, Equation::Add,
		Factor::DstAlpha, Factor::Zero, Equation::Add
	);

	inline constexpr BlendMode BlendMode::Additive = BlendMode(
		Factor::One, Factor::One, Equation::Add,
		Factor::One, Factor::One, Equation::Add
	);

	inline constexpr BlendMode BlendMode::Opaque = BlendMode(
		Factor::One, Factor::Zero, Equation::Add,
		Factor::One, Factor::Zero, Equation::Add
	);

	inline constexpr BlendMode BlendMode::Invert = BlendMode(
		Factor::OneMinusDstColor, Factor::Zero, Equation::Add,
		Factor::OneMinusDstAlpha, Factor::Zero, Equation::Add
	);
}