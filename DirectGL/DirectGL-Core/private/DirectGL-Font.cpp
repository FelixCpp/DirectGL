module;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <format>

module DirectGL;

import :Font;

namespace DGL
{

	inline static constexpr Math::Uint2 GLYPH_ATLAS_SIZE = { 512, 512 };

	std::unique_ptr<Font> Font::CreateFromFile(const std::filesystem::path& filepath, const uint32_t textSize)
	{
		// Construct the font library.
		auto library = CreateFreeTypeLibrary();
		if (library == nullptr)
		{
			Error("Failed to initialize Freetype library");
			return nullptr;
		}

		// Load the font face from file.
		auto fontFace = CreateFreeTypeFace(library, filepath);
		if (fontFace == nullptr)
		{
			Error(std::format("Failed to load font face from file: {}", filepath.string()));
			return nullptr;
		}

		// Set the desired pixel size for the font.
		FT_Set_Pixel_Sizes(fontFace.get(), 0, textSize);

		// Create the font object.
		return std::unique_ptr<Font>(new Font(std::move(library), std::move(fontFace), textSize));
	}

	const Glyph* Font::GetGlyph(const char32_t character)
	{
		// Check if the character has already been loaded.
		const auto itr = m_Glyphs.find(character);
		if (itr != m_Glyphs.end())
		{
			// In this case we can simply return the existing glyph.
			return &itr->second;
		}

		// At this point the glyph is unknown, so we need to load it.
		const FT_UInt glyphIndex = FT_Get_Char_Index(m_FontFace.get(), character);
		if (glyphIndex == 0)
		{
			// Character is not available in the font.
			return nullptr;
		}

		// Load the glyph from the font face.
		FT_Error error = FT_Load_Glyph(m_FontFace.get(), glyphIndex, FT_LOAD_DEFAULT);
		if (error != FT_Err_Ok)
		{
			Error(std::format("Failed to load glyph for character U+{:04X}", static_cast<uint32_t>(character)));
			return nullptr;
		}

		error = FT_Render_Glyph(m_FontFace->glyph, FT_RENDER_MODE_NORMAL);
		if (error != FT_Err_Ok)
		{
			Error(std::format("Failed to render glyph for character U+{:04X}", static_cast<uint32_t>(character)));
			return nullptr;
		}

		// Gather some glyph information we'll need later
		const FT_Bitmap sourceBitmap = m_FontFace->glyph->bitmap;
		const FT_UInt bitmapWidth = sourceBitmap.width;
		const FT_UInt bitmapHeight = sourceBitmap.rows;
		const FT_Bytes bitmapData = sourceBitmap.buffer;

		// Ensure there's at least room for one glyph in the atlas.
		// If not, we can't proceed.
		if (bitmapWidth > GLYPH_ATLAS_SIZE.X or bitmapHeight > GLYPH_ATLAS_SIZE.Y)
		{
			Error(std::format("Glyph for character U+{:04X} is too large to fit in the atlas", static_cast<uint32_t>(character)));
			return nullptr;
		}

		Page* currentPage = m_Pages.empty() ? nullptr : &m_Pages.back();
		const bool needsNewRow = currentPage != nullptr and (m_GlyphCursor.X + bitmapWidth) > currentPage->TextureAtlas->GetSize().Y;
		const bool needsNewPage = currentPage == nullptr or (needsNewRow and (m_GlyphCursor.Y + currentPage->CurrentRowHeight + bitmapHeight) > currentPage->TextureAtlas->GetSize().Y);

		if (needsNewRow)
		{
			// Move to the next row in the current page.
			currentPage->RowHeights.push_back(currentPage->CurrentRowHeight);

			// Reset the glyph cursor X position and advance the Y position.
			m_GlyphCursor.X = 0;
			m_GlyphCursor.Y += currentPage->CurrentRowHeight;

			Info(std::format("Moved to new row at Y={} in atlas page {}", m_GlyphCursor.Y, std::distance(m_Pages.data(), currentPage)));
		}

		if (needsNewPage)
		{
			// Insert a new page into the atlas.
			currentPage = &m_Pages.emplace_back(Page {
				.TextureAtlas = Image2D::CreateFromMemory(GLYPH_ATLAS_SIZE.X, GLYPH_ATLAS_SIZE.Y, nullptr, ImageFormat::Gray8),
				.RowHeights = {},
				.CurrentRowHeight = 0,
			});

			Info(std::format("Created new atlas page {}", m_Pages.size() - 1));

			// Reset the glyph cursor position.
			m_GlyphCursor = { 0, 0 };
		}

		// Update the current row height if needed.
		if (bitmapHeight > currentPage->CurrentRowHeight)
		{
			currentPage->CurrentRowHeight = bitmapHeight;
			Info(std::format("Updated current row height to {} in atlas page {}", currentPage->CurrentRowHeight, std::distance(m_Pages.data(), currentPage)));
		}

		const Math::Uint2 pageSize = currentPage->TextureAtlas->GetSize();

		// Create the boundary for this glyph inside the atlas.
		const Math::UintBoundary glyphBoundary = Math::UintBoundary::FromLTWH(m_GlyphCursor.X, m_GlyphCursor.Y, bitmapWidth, bitmapHeight);

		// Create the UV boundary for this glyph inside the atlas.
		const Math::FloatBoundary glyphUVBoundary = Math::FloatBoundary::FromLTWH(
			static_cast<float>(glyphBoundary.Left) / static_cast<float>(pageSize.X),
			static_cast<float>(glyphBoundary.Top) / static_cast<float>(pageSize.Y),
			static_cast<float>(glyphBoundary.Width) / static_cast<float>(pageSize.X),
			static_cast<float>(glyphBoundary.Height) / static_cast<float>(pageSize.Y)
		);

		// Now we're ready to copy the glyph information into our atlas.
		// This will be done by copying the bitmap data into the texture atlas at the current glyph cursor position.
		currentPage->TextureAtlas->Update(glyphBoundary.Left, glyphBoundary.Top, glyphBoundary.Width, glyphBoundary.Height, bitmapData);

		// Advance the glyph cursor position.
		m_GlyphCursor.X += bitmapWidth;

		// Create the glyph object and store it in our glyph map.
		Glyph glyph = {
			.AtlasIndex = static_cast<uint32_t>(std::distance(m_Pages.data(), currentPage)),
			.AtlasRect = glyphBoundary,
			.AtlasRectUV = glyphUVBoundary,
			.Bearing = { m_FontFace->glyph->bitmap_left, m_FontFace->glyph->bitmap_top },
			.Advance = static_cast<uint32_t>(m_FontFace->glyph->advance.x >> 6),
		};

		Info(std::format("Loaded glyph for character {} into atlas page {}, rect L:{} T:{} W:{} H:{}", (char)character, glyph.AtlasIndex, glyph.AtlasRect.Left, glyph.AtlasRect.Top, glyph.AtlasRect.Width, glyph.AtlasRect.Height));
		
		// Insert the glyph into our map.
		const auto insertion = m_Glyphs.emplace(character, std::move(glyph));
		if (not insertion.second)
		{
			Error(std::format("Failed to insert glyph for character U+{:04X} into glyph map", static_cast<uint32_t>(character)));
			return nullptr;
		}

		return &insertion.first->second;
	}

	size_t Font::GetPageCount() const
	{
		return m_Pages.size();
	}

	const Font::Page& Font::GetPage(const size_t index) const
	{
		return m_Pages.at(index);
	}

	void Font::FreeTypeDeleter::operator()(const FT_Library library) const
	{
		if (library != nullptr)
		{
			FT_Done_FreeType(library);
		}
	}

	void Font::FreeTypeDeleter::operator()(const FT_Face face) const
	{
		if (face != nullptr)
		{
			FT_Done_Face(face);
		}
	}
	
	Font::FreeTypeLibraryPtr Font::CreateFreeTypeLibrary()
	{
		FT_Library library = nullptr;
		FT_Init_FreeType(&library);
		return FreeTypeLibraryPtr(library);
	}

	Font::FreeTypeFacePtr Font::CreateFreeTypeFace(const FreeTypeLibraryPtr& library, const std::filesystem::path& filepath)
	{
		FT_Face fontFace = nullptr;
		FT_New_Face(library.get(), filepath.string().c_str(), 0, &fontFace);
		return FreeTypeFacePtr(fontFace);
	}

	Font::Font(FreeTypeLibraryPtr library, FreeTypeFacePtr fontFace, const uint32_t textSize):
		m_FontLibrary(std::move(library)),
		m_FontFace(std::move(fontFace)),
		m_TextSize(textSize)
	{
	}
}
