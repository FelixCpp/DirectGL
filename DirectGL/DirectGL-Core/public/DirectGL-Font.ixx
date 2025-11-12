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

export module DirectGL:Font;

import DirectGL.Math;

export namespace DGL
{
	struct Glyph
	{
		Math::Uint2			TextureSize;
		Math::Uint2			Advance;
		Math::IntBoundary	Bearing;
		uint32_t			CharacterHeight;
		Math::FloatBoundary TextureCoordinates;
	};

	class Font
	{
	private:

		struct FontProperties
		{
			GLuint				TextureId;
			uint32_t			FontSize;
			uint32_t			LineHeight;
			std::vector<Glyph>	Glyphs;
		};

	public:

		static std::unique_ptr<Font> CreateFromFile(const std::filesystem::path& filepath, uint32_t fontSize);
		static std::unique_ptr<Font> CreateFromMemory(const std::span<const uint8_t>& data, uint32_t fontSize);

		~Font();

		Math::Float2 ComputeTextSize(std::string_view text, float textSize) const;
		uint32_t GetLineHeight() const;
		float GetPixelScale(float fontSize) const;
		const Glyph* GetGlyph(char character) const;
		GLuint GetTextureId() const;

	private:

		explicit Font(const FontProperties& properties);

		static std::optional<FontProperties> CreateFontProperties(FT_Face fontFace, uint32_t fontSize);


		GLuint				m_TextureId;
		uint32_t			m_FontSize;
		uint32_t			m_LineHeight;
		std::vector<Glyph>	m_Glyphs;

	};
}