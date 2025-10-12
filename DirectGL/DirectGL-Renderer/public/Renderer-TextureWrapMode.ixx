// Project Name : DirectGL-Renderer
// File Name    : Renderer-TextureWrapMode.ixx
// Author       : Felix Busch
// Created Date : 2025/10/07

export module DirectGL.Renderer:TextureWrapMode;

export namespace DGL::Renderer
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

		constexpr bool operator == (const TextureWrapMode&) const = default;
		constexpr bool operator != (const TextureWrapMode&) const = default;

		static const TextureWrapMode Repeat;
		static const TextureWrapMode MirroredRepeat;
		static const TextureWrapMode ClampToEdge;
		static const TextureWrapMode ClampToBorder;
	};
}

namespace DGL::Renderer
{
	inline constexpr TextureWrapMode TextureWrapMode::Repeat = { .Horizontal = TextureWrapModeId::Repeat, .Vertical = TextureWrapModeId::Repeat };
	inline constexpr TextureWrapMode TextureWrapMode::MirroredRepeat = { .Horizontal = TextureWrapModeId::MirroredRepeat, .Vertical = TextureWrapModeId::MirroredRepeat };
	inline constexpr TextureWrapMode TextureWrapMode::ClampToEdge = { .Horizontal = TextureWrapModeId::ClampToEdge, .Vertical = TextureWrapModeId::ClampToEdge };
	inline constexpr TextureWrapMode TextureWrapMode::ClampToBorder = { .Horizontal = TextureWrapModeId::ClampToBorder, .Vertical = TextureWrapModeId::ClampToBorder };
}