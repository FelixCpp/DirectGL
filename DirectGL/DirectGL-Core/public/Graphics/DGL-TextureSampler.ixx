// Project Name : DirectGL
// File Name    : DGL-TextureSampler.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

export module DGL:TextureSampler;

export namespace DGL
{
	enum class TextureWrapModeId
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder
	};

	struct TextureWrapMode
	{
		TextureWrapModeId Horizontal;
		TextureWrapModeId Vertical;

		static const TextureWrapMode Repeat;
		static const TextureWrapMode MirroredRepeat;
		static const TextureWrapMode ClampToEdge;
		static const TextureWrapMode ClampToBorder;
	};

	enum class TextureFilterModeId
	{
		Nearest,
		Linear
	};

	struct TextureFilterMode
	{
		TextureFilterModeId Minification;
		TextureFilterModeId Magnification;

		static const TextureFilterMode Nearest;
		static const TextureFilterMode Linear;
	};

	struct TextureSampler
	{
		virtual ~TextureSampler() = default;

		virtual void SetWrapMode(TextureWrapMode mode) = 0;
		[[nodiscard]] virtual TextureWrapMode GetWrapMode() const = 0;

		virtual void SetFilterMode(TextureFilterMode mode) = 0;
		[[nodiscard]] virtual TextureFilterMode GetFilterMode() const = 0;
	};
}

namespace DGL
{
	inline constexpr TextureWrapMode TextureWrapMode::Repeat = { .Horizontal = TextureWrapModeId::Repeat, .Vertical = TextureWrapModeId::Repeat };
	inline constexpr TextureWrapMode TextureWrapMode::MirroredRepeat = { .Horizontal = TextureWrapModeId::MirroredRepeat, .Vertical = TextureWrapModeId::MirroredRepeat };
	inline constexpr TextureWrapMode TextureWrapMode::ClampToEdge = { .Horizontal = TextureWrapModeId::ClampToEdge, .Vertical = TextureWrapModeId::ClampToEdge };
	inline constexpr TextureWrapMode TextureWrapMode::ClampToBorder = { .Horizontal = TextureWrapModeId::ClampToBorder, .Vertical = TextureWrapModeId::ClampToBorder };

	inline constexpr TextureFilterMode TextureFilterMode::Nearest = { .Minification = TextureFilterModeId::Nearest, .Magnification = TextureFilterModeId::Nearest };
	inline constexpr TextureFilterMode TextureFilterMode::Linear = { .Minification = TextureFilterModeId::Linear, .Magnification = TextureFilterModeId::Linear };
}
