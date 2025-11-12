module;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/gl.h>

#include <format>
#include <span>
#include <utility>
#include <string_view>

module DirectGL;

import :Font;

namespace DGL
{
	struct FreeTypeDeleter
	{
		void operator()(const FT_Library library) const
		{
			FT_Done_FreeType(library);
		}

		void operator()(const FT_Face face) const
		{
			FT_Done_Face(face);
		}
	};

	std::unique_ptr<FT_LibraryRec_, FreeTypeDeleter> LoadFontLibrary()
	{
		FT_Library library;
		if (FT_Init_FreeType(&library))
		{
			return nullptr;
		}

		return std::unique_ptr<FT_LibraryRec_, FreeTypeDeleter>(library);
	}

	std::unique_ptr<FT_FaceRec_, FreeTypeDeleter> LoadFontFaceFromFile(const FT_Library library, const std::filesystem::path& filepath)
	{
		FT_Face face;
		if (FT_New_Face(library, filepath.string().c_str(), 0, &face))
		{
			return nullptr;
		}

		return std::unique_ptr<FT_FaceRec_, FreeTypeDeleter>(face);
	}

	std::unique_ptr<FT_FaceRec_, FreeTypeDeleter> LoadFontFaceFromMemory(const FT_Library library, const std::span<const FT_Byte>& data)
	{
		FT_Face face;
		if (const auto error = FT_New_Memory_Face(library, data.data(), data.size(), 0, &face); error != FT_Err_Ok)
		{
			Warning(std::format("Warning: {}", error));

			return nullptr;
		}

		return std::unique_ptr<FT_FaceRec_, FreeTypeDeleter>(face);
	}

	std::unique_ptr<Font> Font::CreateFromFile(const std::filesystem::path& filepath, uint32_t fontSize)
	{
		const std::unique_ptr library = LoadFontLibrary();
		if (library == nullptr)
		{
			Error("Failed to initialize FreeType library.");
			return nullptr;
		}

		const std::unique_ptr fontFace = LoadFontFaceFromFile(library.get(), filepath);
		if (fontFace == nullptr)
		{
			Error(std::format("Failed to load font face from file: {}", filepath.string()));
			return nullptr;
		}

		Info(std::format("Loaded font: {} ({})", fontFace->family_name, fontFace->style_name));

		const auto properties = CreateFontProperties(fontFace.get(), fontSize);
		if (not properties.has_value())
		{
			Error("Failed to create font.");
			return nullptr;
		}

		return std::unique_ptr<Font>(new Font(properties.value()));
	}

	std::unique_ptr<Font> Font::CreateFromMemory(const std::span<const uint8_t>& data, uint32_t fontSize)
	{
		const std::unique_ptr library = LoadFontLibrary();
		if (library == nullptr)
		{
			Error("Failed to initialize FreeType library.");
			return nullptr;
		}

		const std::unique_ptr fontFace = LoadFontFaceFromMemory(library.get(), std::span{ data.data(), data.size() });
		if (fontFace == nullptr)
		{
			Error("Failed to load font face from memory.");
			return nullptr;
		}

		const auto properties = CreateFontProperties(fontFace.get(), fontSize);
		if (not properties.has_value())
		{
			Error("Failed to create font.");
			return nullptr;
		}

		return std::unique_ptr<Font>(new Font(properties.value()));
	}

	Font::~Font()
	{
		glDeleteTextures(1, &m_TextureId);
	}

	Math::Float2 Font::ComputeTextSize(const std::string_view text, const float textSize) const
	{
		Math::Float2 size = Math::Float2::Zero;

		const float pixelScale = GetPixelScale(textSize);
		float currentLineWidth = 0.0f;
		for (const char character : text)
		{
			if (character == '\n')
			{
				currentLineWidth = 0.0f;
				size.Y += static_cast<float>(m_LineHeight) * pixelScale;
				continue;
			}

			const Glyph* glyph = GetGlyph(character);
			if (glyph == nullptr)
			{
				glyph = GetGlyph('?');

				if (glyph == nullptr)
				{
					continue;
				}
			}

			currentLineWidth += static_cast<float>(glyph->Advance.X) * pixelScale;
			size.X = std::max(size.X, currentLineWidth);
		}

		return size;
	}

	uint32_t Font::GetLineHeight() const
	{
		return m_LineHeight;
	}

	float Font::GetPixelScale(const float fontSize) const
	{
		return fontSize / static_cast<float>(m_FontSize);
	}

	const Glyph* Font::GetGlyph(const char character) const
	{
		if (character - 32 >= static_cast<char>(m_Glyphs.size()))
		{
			return nullptr;
		}

		const size_t glyphIndex = static_cast<size_t>(character) - 32;
		return &m_Glyphs[glyphIndex];
	}

	GLuint Font::GetTextureId() const
	{
		return m_TextureId;
	}

	Font::Font(const FontProperties& properties):
		m_TextureId(properties.TextureId),
		m_FontSize(properties.FontSize),
		m_LineHeight(properties.LineHeight),
		m_Glyphs(properties.Glyphs)
	{
	}

	std::optional<Font::FontProperties> Font::CreateFontProperties(const FT_Face fontFace, const uint32_t fontSize)
	{
		FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

		constexpr uint32_t textureWidth = 1024;		// Width of the font texture atlas
		constexpr uint32_t textureHeight = 1024;	// Height of the font texture atlas
		constexpr uint32_t padding = 2;				// Padding between glyphs in the texture atlas

		std::vector<uint8_t> textureData(textureWidth * textureHeight, 0);

		uint32_t largestGlyphHeightInRow = 0;	// Height of the largest glyph in the current row
		uint32_t largestGlyphHeightInAtlas = 0;	// The largest height of all glyphs
		uint32_t column	= 0; // Current X offset in the texture atlas
		uint32_t row	= 0; // Current Y offset in the texture atlas

		std::vector<Glyph> glyphs;

		for (FT_ULong glyphIndex = 32; glyphIndex < 32 + 95; ++glyphIndex)
		{
			if (FT_Load_Char(fontFace, glyphIndex, FT_LOAD_RENDER))
			{
				Warning(std::format("Could not load glyph index {}", glyphIndex));
				continue;
			}

			if (FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_SDF))
			{
				Warning(std::format("Could not render glyph index {}", glyphIndex));
				continue;
			}

			// Update the largest glyph height in the current row
			largestGlyphHeightInRow = std::max(largestGlyphHeightInRow, fontFace->glyph->bitmap.rows);
			

			// Wrap around if the glyph does not fit in the current row
			if (column + fontFace->glyph->bitmap.width + padding > textureWidth)
			{
				column = padding;
				row += largestGlyphHeightInRow + padding;
				largestGlyphHeightInRow = 0;
			}

			// Copy the glyph bitmap data into the texture atlas data
			auto bmp = fontFace->glyph->bitmap;
			int pitch = std::abs(bmp.pitch);
			for (FT_UInt y = 0; y < bmp.rows; ++y)
			{
				int rowOffset = (bmp.pitch > 0) ? y * pitch : (bmp.rows - 1 - y) * pitch;

				for (FT_UInt x = 0; x < bmp.width; ++x)
				{
					const uint32_t textureX = column + x;
					const uint32_t textureY = row + y;

					if (textureX >= textureWidth || textureY >= textureHeight)
						continue; // Sicherheit!

					textureData[textureY * textureWidth + textureX] = bmp.buffer[rowOffset + x];
				}
			}
			//for (FT_UInt y = 0; y < fontFace->glyph->bitmap.rows; ++y)
			//{
			//	for (FT_UInt x = 0; x < fontFace->glyph->bitmap.width; ++x)
			//	{
			//		const uint32_t textureX = column + x;
			//		const uint32_t textureY = row + y;
			//		textureData[textureY * textureWidth + textureX] = fontFace->glyph->bitmap.buffer[y * fontFace->glyph->bitmap.pitch + x];
			//	}
			//}

			// Compute the glyph height in pixels
			const uint32_t glyphHeight = fontFace->size->metrics.height >> 6;

			// Update the largest glyph height in the atlas
			largestGlyphHeightInAtlas = std::max(largestGlyphHeightInAtlas, glyphHeight);

			// Compute the texture coordinates for this glyph (u, v)
			const float uvTop = static_cast<float>(row) / static_cast<float>(textureHeight);
			const float uvLeft = static_cast<float>(column) / static_cast<float>(textureWidth);
			const float uvWidth = static_cast<float>(fontFace->glyph->bitmap.width) / static_cast<float>(textureWidth);
			const float uvHeight = static_cast<float>(fontFace->glyph->bitmap.rows) / static_cast<float>(textureHeight);

			const int32_t bearingLeft = fontFace->glyph->bitmap_left;
			const int32_t bearingTop = fontFace->glyph->bitmap_top;
			const int32_t bearingWidth = fontFace->glyph->bitmap.width - bearingLeft;
			const int32_t bearingHeight = fontFace->glyph->bitmap.rows - bearingTop;

			glyphs.emplace_back(Glyph {
				.TextureSize = { fontFace->glyph->bitmap.width, fontFace->glyph->bitmap.rows },
				.Advance = { static_cast<uint32_t>(fontFace->glyph->advance.x >> 6), static_cast<uint32_t>(fontFace->glyph->advance.y >> 6) },
				.Bearing = Math::IntBoundary::FromLTWH(bearingLeft, bearingTop, bearingWidth, bearingHeight),
				.CharacterHeight = glyphHeight,
				.TextureCoordinates = Math::FloatBoundary::FromLTWH(uvLeft, uvTop, uvWidth, uvHeight)
			});

			// Increment the column position for the next glyph
			column += fontFace->glyph->bitmap.width + padding;
		}

		// Generate an opengl texture
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_R8, textureWidth, textureHeight);
		glTextureSubImage2D(textureId, 0, 0, 0, textureWidth, textureHeight, GL_RED, GL_UNSIGNED_BYTE, textureData.data());
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(textureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(textureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateTextureMipmap(textureId);

		return FontProperties {
			.TextureId = textureId,
			.FontSize = fontSize,
			.LineHeight = largestGlyphHeightInAtlas,
			.Glyphs = std::move(glyphs),
		};
	}
}