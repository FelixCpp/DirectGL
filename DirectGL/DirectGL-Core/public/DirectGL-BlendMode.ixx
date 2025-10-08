// Project Name : DirectGL-Core
// File Name    : DirectGL-BlendMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/08

export module DirectGL:BlendMode;

import DirectGL.Renderer;

export namespace DGL
{
	using BlendMode = Renderer::BlendMode;
}

export namespace DGL::BlendModes
{
	inline constexpr auto Alpha = Renderer::BlendMode(
		Renderer::BlendMode::Factor::SrcAlpha,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Additive = Renderer::BlendMode(
		Renderer::BlendMode::Factor::SrcAlpha,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Subtractive = Renderer::BlendMode(
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Factor::OneMinusSrcColor,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Dividing = Renderer::BlendMode(
		Renderer::BlendMode::Factor::DstColor,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::DstAlpha,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Multiply = Renderer::BlendMode(
		Renderer::BlendMode::Factor::DstColor,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::DstAlpha,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Screen = Renderer::BlendMode(
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::OneMinusSrcColor,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Opaque = Renderer::BlendMode(
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Invert = Renderer::BlendMode(
		Renderer::BlendMode::Factor::OneMinusDstColor,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::OneMinusDstAlpha,
		Renderer::BlendMode::Factor::Zero,
		Renderer::BlendMode::Equation::Add
	);

	inline constexpr auto Darken = Renderer::BlendMode(
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Min,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Min
	);

	inline constexpr auto Lighten = Renderer::BlendMode(
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Max,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::Max
	);

	inline constexpr auto Difference = Renderer::BlendMode(
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::ReverseSubtract,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Factor::One,
		Renderer::BlendMode::Equation::ReverseSubtract
	);

	inline constexpr auto Exclusion = Renderer::BlendMode(
		Renderer::BlendMode::Factor::OneMinusDstAlpha,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add,
		Renderer::BlendMode::Factor::OneMinusDstAlpha,
		Renderer::BlendMode::Factor::OneMinusSrcAlpha,
		Renderer::BlendMode::Equation::Add
	);

	
}