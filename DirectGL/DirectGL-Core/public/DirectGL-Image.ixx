// Project Name : DirectGL
// File Name    : DirectGL-Image2D.ixx
// Author       : Felix Busch
// Created Date : 2025/11/15

module;

#include <memory>
#include <cstdint>
#include <filesystem>

#include <glad/gl.h>

export module DirectGL:Image2D;

import DirectGL.Math;

export namespace DGL
{
	class Image2D
	{
	public:

		static std::unique_ptr<Image2D> CreateFromFile(const std::filesystem::path& filepath);
		static std::unique_ptr<Image2D> CreateFromMemory(uint32_t width, uint32_t height, const uint8_t* data);

		~Image2D();

		void Update(uint32_t left, uint32_t top, uint32_t width, uint32_t height, const uint8_t* data);

		std::unique_ptr<Image2D> Clone() const;
		std::unique_ptr<Image2D> CreateView(uint32_t left, uint32_t top, uint32_t width, uint32_t height) const;

		std::unique_ptr<uint8_t[]> QueryPixelData() const;

		const Math::Uint2& GetSize() const;
		uint32_t GetTextureId() const;

	private:

		explicit Image2D(uint32_t textureId, Math::Uint2 size);

		GLuint		m_TextureId;
		Math::Uint2 m_Size;

	};
}