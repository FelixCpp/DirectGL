// Project Name : DirectGL
// File Name    : DirectGL-Font.ixx
// Author       : Felix Busch
// Created Date : 2025/11/16

module;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <vector>
#include <memory>
#include <filesystem>
#include <unordered_map>

export module DirectGL:Font;

import :Image2D;
import :Glyph;

export namespace DGL
{
	/// @brief This class represents a font, consisting of multiple pages of glyphs.
	///
	/// Glyphs are lazy-loaded as needed from font files.
	class Font
	{
	public:

		/// @brief This class represents a single page of a font atlas.
		///
		/// A page consists of a texture atlas containing multiple glyphs.
		struct Page
		{
			std::unique_ptr<Image2D>	TextureAtlas;			//!< The texture atlas containing the glyphs for this page
			std::vector<uint32_t>		RowHeights;				//!< List of heights for each row in the atlas
			uint32_t					CurrentRowHeight;		//!< The height of the current row being filled
		};

		/// @brief Construct a new Font object from a font file.
		///
		/// Tries to load the font from the given file path
		/// with the given text size.
		/// 
		/// If the font could not be loaded, nullptr is returned.
		///
		/// @param filepath The path to the font file.
		/// @param textSize The size of the text to load the font at.
		/// 
		/// @return A unique pointer to the created Font object, or nullptr if loading failed.
		static std::unique_ptr<Font> CreateFromFile(const std::filesystem::path& filepath, uint32_t textSize);
		
		/// @brief Retrieve a glyph for the given character.
		///
		/// In case the glyph has not been loaded yet, it will be
		/// loaded and added to the font atlas.
		///
		/// If the character is not available in the font, nullptr
		/// is returned.
		///
		/// @param character The character to retrieve the glyph for.
		/// @return Pointer to the glyph, or nullptr if the character is not available.
		const Glyph* GetGlyph(char32_t character);

		/// @brief Get the number of pages in the font atlas.
		///
		/// This indicates how many texture atlases are used to
		/// store the glyphs of this font.
		/// Note that this number can grow as more glyphs are loaded.
		/// 
		/// @return The number of pages in the font atlas.
		size_t GetPageCount() const;

		/// @brief Get the page at the given index.
		///
		/// @param index A zero-based index of the page to retrieve.
		/// 
		/// @return A reference to the requested page.
		const Page& GetPage(size_t index) const;

	private:

		/// @brief Wrapper deleter for FreeType objects.
		struct FreeTypeDeleter
		{
			void operator()(FT_Library library) const;
			void operator()(FT_Face face) const;
		};

		/// @brief Handy type definitions for FreeType smart pointers.
		using FreeTypeLibraryPtr = std::unique_ptr<std::remove_pointer_t<FT_Library>, FreeTypeDeleter>;
		using FreeTypeFacePtr = std::unique_ptr<std::remove_pointer_t<FT_Face>, FreeTypeDeleter>;

		static FreeTypeLibraryPtr CreateFreeTypeLibrary();
		static FreeTypeFacePtr CreateFreeTypeFace(const FreeTypeLibraryPtr& library, const std::filesystem::path& filepath);

	private:

		explicit Font(FreeTypeLibraryPtr library, FreeTypeFacePtr fontFace, uint32_t textSize);

		FreeTypeLibraryPtr	m_FontLibrary;	//!< The FreeType font library instance
		FreeTypeFacePtr		m_FontFace;		//!< The FreeType font face instance
		Math::Uint2			m_GlyphCursor;	//!< The current position of the glyph cursor in the current atlas
		uint32_t			m_TextSize;		//!< The size of the text in pixels

		std::vector<Page>					m_Pages;	//!< The pages of the font atlas
		std::unordered_map<char32_t, Glyph>	m_Glyphs;	//!< The loaded glyphs

	};
}