// Project Name : DirectGL
// File Name    : DirectGL-ImageSamplerWrapMode.ixx
// Author       : Felix Busch
// Created Date : 2025/11/15

export module DirectGL:ImageSamplerWrapMode;

export namespace DGL
{
	enum class ImageSamplerWrapModeType
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	struct ImageSamplerWrapMode
	{
		ImageSamplerWrapModeType WrapS;
		ImageSamplerWrapModeType WrapT;

		constexpr bool operator == (const ImageSamplerWrapMode& other) const = default;
		constexpr bool operator != (const ImageSamplerWrapMode& other) const = default;

		static const ImageSamplerWrapMode Repeat;
		static const ImageSamplerWrapMode MirroredRepeat;
		static const ImageSamplerWrapMode ClampToEdge;
		static const ImageSamplerWrapMode ClampToBorder;
	};
}

namespace DGL
{
	const ImageSamplerWrapMode ImageSamplerWrapMode::Repeat			= { .WrapS = ImageSamplerWrapModeType::Repeat, .WrapT = ImageSamplerWrapModeType::Repeat };
	const ImageSamplerWrapMode ImageSamplerWrapMode::MirroredRepeat	= { .WrapS = ImageSamplerWrapModeType::MirroredRepeat, .WrapT = ImageSamplerWrapModeType::MirroredRepeat };
	const ImageSamplerWrapMode ImageSamplerWrapMode::ClampToEdge	= { .WrapS = ImageSamplerWrapModeType::ClampToEdge, .WrapT = ImageSamplerWrapModeType::ClampToEdge };
	const ImageSamplerWrapMode ImageSamplerWrapMode::ClampToBorder	= { .WrapS = ImageSamplerWrapModeType::ClampToBorder, .WrapT = ImageSamplerWrapModeType::ClampToBorder };
}