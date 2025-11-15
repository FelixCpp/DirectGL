// Project Name : DirectGL
// File Name    : DirectGL-Font.ixx
// Author       : Felix Busch
// Created Date : 2025/11/12

module;

#include <glad/gl.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <memory>
#include <filesystem>
#include <span>
#include <optional>
#include <string_view>
#include <unordered_map>

export module DirectGL:Font;

import DirectGL.Math;

export namespace DGL
{

	struct Glyph
	{
		Math::Uint2			Size;           //< Width, Height
		Math::Int2			Bearing;        //< Offset from baseline to left/top of glyph
		Math::FloatBoundary	UVRect;         //< UV coordinates in the font texture
		uint32_t			Advance;        //< Offset to advance to next glyph
	};

	class Font
	{
	public:

		static std::unique_ptr<Font> CreateFromFile(const std::filesystem::path& filepath, uint32_t fontSize);
		static std::unique_ptr<Font> CreateFromMemory(const std::span<const uint8_t>& data, uint32_t fontSize);

		float GetPixelScale(float textSize) const;
		const Glyph* GetGlyph(char32_t character) const;
		uint32_t GetTextureId() const;

	private:

		explicit Font(
			const std::unordered_map<char32_t, Glyph>& glyphs,
			uint32_t textureId,
			uint32_t lineHeight
		);

		std::unordered_map<char32_t, Glyph>	m_Glyphs;
		uint32_t m_TextureId;
		uint32_t m_LineHeight;

	};
}