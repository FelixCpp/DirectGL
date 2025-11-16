// Project Name : DirectGL
// File Name    : DirectGL-ImageSamplerFilterMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/15

export module DirectGL:ImageSamplerFilterMode;

export namespace DGL
{
	enum class ImageSamplerFilterModeType
	{
		Nearest,
		Linear,
	};

	struct ImageSamplerFilterMode
	{
		ImageSamplerFilterModeType	MinFilter;
		ImageSamplerFilterModeType	MagFilter;

		constexpr bool operator == (const ImageSamplerFilterMode& other) const = default;
		constexpr bool operator != (const ImageSamplerFilterMode& other) const = default;

		static const ImageSamplerFilterMode Nearest;
		static const ImageSamplerFilterMode Linear;
	};
}

namespace DGL
{
	const ImageSamplerFilterMode ImageSamplerFilterMode::Nearest	= { .MinFilter = ImageSamplerFilterModeType::Nearest, .MagFilter = ImageSamplerFilterModeType::Nearest };
	const ImageSamplerFilterMode ImageSamplerFilterMode::Linear	= { .MinFilter = ImageSamplerFilterModeType::Linear, .MagFilter = ImageSamplerFilterModeType::Linear };
}