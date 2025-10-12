// Project Name : DirectGL-Renderer
// File Name    : Renderer-TextureFilterMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Renderer:TextureFilterMode;

export namespace DGL::Renderer
{
	enum class TextureFilterModeId
	{
		Nearest,
		Linear,
	};

	struct TextureFilterMode
	{
		TextureFilterModeId Minification;
		TextureFilterModeId Magnification;

		constexpr bool operator == (const TextureFilterMode&) const = default;
		constexpr bool operator != (const TextureFilterMode&) const = default;

		static const TextureFilterMode Nearest;
		static const TextureFilterMode Linear;
	};
}

namespace DGL::Renderer
{
	inline constexpr TextureFilterMode TextureFilterMode::Nearest = { .Minification = TextureFilterModeId::Nearest, .Magnification = TextureFilterModeId::Nearest };
	inline constexpr TextureFilterMode TextureFilterMode::Linear = { .Minification = TextureFilterModeId::Linear, .Magnification = TextureFilterModeId::Linear };
}
