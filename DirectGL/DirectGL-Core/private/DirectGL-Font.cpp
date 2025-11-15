module;

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <glad/gl.h>

#include <format>
#include <span>
#include <utility>
#include <string_view>

module DirectGL;

import :Font;

namespace DGL
{
	std::unique_ptr<Font> Font::CreateFromFile(const std::filesystem::path& filepath, const uint32_t fontSize)
	{
		constexpr auto atlasSize = Math::Uint2{ 1024, 1024 };
		constexpr auto glyphPadding = 2u;

		FT_Library library;
		FT_Init_FreeType(&library);

		FT_Face face;
		FT_New_Face(library, filepath.string().c_str(), 0, &face);

		FT_Set_Pixel_Sizes(face, 0, fontSize);

		const auto atlasData = std::make_unique<uint8_t[]>(atlasSize.X * atlasSize.Y);
		std::unordered_map<char32_t, Glyph> glyphs;
		auto atlasPen = Math::Uint2::Zero;
		uint32_t maxRowHeight = 0;
		const uint32_t lineHeight = face->size->metrics.height >> 6;

		const auto f = face->glyph->metrics.horiBearingX;

		for (char32_t c = ' '; c < 127; ++c)
		{
			if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
			{
				continue;
			}

			if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
			{
				continue;
			}

			maxRowHeight = std::max(maxRowHeight, face->glyph->bitmap.rows);

			if (atlasPen.X + face->glyph->bitmap.width >= atlasSize.X)
			{
				atlasPen.X = glyphPadding;
				atlasPen.Y += maxRowHeight;
			}

			for (uint32_t y = 0; y < face->glyph->bitmap.rows; ++y)
			{
				for (uint32_t x = 0; x < face->glyph->bitmap.width; ++x)
				{
					const uint8_t value = face->glyph->bitmap.buffer[y * face->glyph->bitmap.pitch + x];
					const uint32_t pixelIndex = (atlasPen.Y + y) * atlasSize.X + (atlasPen.X + x);
					atlasData[pixelIndex] = value;
				}
			}

			const float uvLeft = static_cast<float>(atlasPen.X) / static_cast<float>(atlasSize.X);
			const float uvTop = static_cast<float>(atlasPen.Y) / static_cast<float>(atlasSize.Y);
			const float uvWidth = static_cast<float>(face->glyph->bitmap.width) / static_cast<float>(atlasSize.X);
			const float uvHeight = static_cast<float>(face->glyph->bitmap.rows) / static_cast<float>(atlasSize.Y);

			Glyph glyph = {
				.Size = Math::Uint2{ face->glyph->bitmap.width, face->glyph->bitmap.rows },
				.Bearing = Math::Int2{ face->glyph->bitmap_left, face->glyph->bitmap_top },
				.UVRect = Math::FloatBoundary::FromLTWH(uvLeft, uvTop, uvWidth, uvHeight),
				.Advance = static_cast<uint32_t>(face->glyph->advance.x >> 6),
			};

			glyphs.emplace(c, std::move(glyph));
			atlasPen.X += face->glyph->bitmap.width + glyphPadding;
		}

		uint32_t textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_R8, atlasSize.X, atlasSize.Y);
		glTextureSubImage2D(textureId, 0, 0, 0, atlasSize.X, atlasSize.Y, GL_RED, GL_UNSIGNED_BYTE, atlasData.get());
		glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		return std::unique_ptr<Font>(new Font(glyphs, textureId, lineHeight));
	}

	std::unique_ptr<Font> Font::CreateFromMemory(const std::span<const uint8_t>& data, uint32_t fontSize)
	{
		return nullptr;
	}

	float Font::GetPixelScale(float textSize) const
	{
		return textSize / static_cast<float>(m_LineHeight);
	}


	const Glyph* Font::GetGlyph(const char32_t character) const
	{
		const auto it = m_Glyphs.find(character);
		if (it == m_Glyphs.end())
		{
			return nullptr;
		}

		return &it->second;
	}

	uint32_t Font::GetTextureId() const
	{
		return m_TextureId;
	}

	Font::Font(
		const std::unordered_map<char32_t, Glyph>& glyphs,
		const uint32_t textureId,
		const uint32_t lineHeight
	) :	m_Glyphs(glyphs),
		m_TextureId(textureId),
		m_LineHeight(lineHeight)
	{
	}
}