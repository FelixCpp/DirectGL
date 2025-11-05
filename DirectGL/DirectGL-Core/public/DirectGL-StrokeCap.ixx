// Project Name : DirectGL
// File Name    : DirectGL-StrokeCap.ixx
// Author       : Felix Busch
// Created Date : 2025/11/04

export module DirectGL:StrokeCap;

export namespace DGL
{
	enum class StrokeCapType
	{
		Butt,
		Round,
		Square
	};

	struct StrokeCap
	{
		StrokeCapType Start;
		StrokeCapType End;

		static const StrokeCap Butt;
		static const StrokeCap Round;
		static const StrokeCap Square;
	};
}

namespace DGL
{
	inline constexpr StrokeCap StrokeCap::Butt = { .Start = StrokeCapType::Butt, .End = StrokeCapType::Butt };
	inline constexpr StrokeCap StrokeCap::Round = { .Start = StrokeCapType::Round, .End = StrokeCapType::Round };
	inline constexpr StrokeCap StrokeCap::Square = { .Start = StrokeCapType::Square, .End = StrokeCapType::Square };
}