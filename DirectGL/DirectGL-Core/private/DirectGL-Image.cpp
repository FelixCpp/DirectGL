module;

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <memory>
#include <format>

module DirectGL;

import DirectGL.ControlFlow;

import :Image2D;

namespace DGL
{
	constexpr ImageFormat ConvertChannelsToImageFormat(const int channels)
	{
		switch (channels)
		{
			case 4: return ImageFormat::RGBA8;
			case 3: return ImageFormat::RGB8;
			case 1: return ImageFormat::Gray8;
			default: ThrowError(std::format("Unsupported number of image channels: {}", channels));
		}
	}

	constexpr GLenum ConvertImageFormatToGLInternalFormat(const ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA8: return GL_RGBA8;
			case ImageFormat::RGB8: return GL_RGB8;
			case ImageFormat::Gray8: return GL_R8;
			default: ThrowError("Unknown ImageFormat in ConvertImageFormatToGLInternalFormat()");
		}
	}

	constexpr GLenum ConvertImageFormatToGLFormat(const ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA8: return GL_RGBA;
			case ImageFormat::RGB8: return GL_RGB;
			case ImageFormat::Gray8: return GL_RED;
			default: ThrowError("Unknown ImageFormat in ConvertImageFormatToGLFormat()");
		}
	}

	constexpr size_t GetBytesByImageFormat(const ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::RGBA8: return 4;
			case ImageFormat::RGB8: return 3;
			case ImageFormat::Gray8: return 1;
			default: ThrowError("Unknown ImageFormat in GetBytesPerPixel()");
		}
	}

	std::unique_ptr<Image2D> Image2D::CreateFromFile(const std::filesystem::path& filepath)
	{
		int width, height, channels;
		const std::unique_ptr<stbi_uc[], decltype(&stbi_image_free)> data(stbi_load(filepath.string().c_str(), &width, &height, &channels, 0), &stbi_image_free);
		if (data == nullptr)
		{
			Error(std::format("Failed to load image from file: {}", filepath.string()));
			return nullptr;
		}

		return CreateFromMemory(static_cast<uint32_t>(width), static_cast<uint32_t>(height), data.get(), ConvertChannelsToImageFormat(channels));
	}

	std::unique_ptr<Image2D> Image2D::CreateFromMemory(const uint32_t width, const uint32_t height, const uint8_t* data, const ImageFormat imageFormat)
	{
		const Math::Uint2 size = { width, height };

		const GLenum glInternalFormat = ConvertImageFormatToGLInternalFormat(imageFormat);
		const GLenum glFormat = ConvertImageFormatToGLFormat(imageFormat);

		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, glInternalFormat, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, glFormat, GL_UNSIGNED_BYTE, data);

		return std::unique_ptr<Image2D>(new Image2D(textureId, size, imageFormat));
	}

	Image2D::~Image2D()
	{
		if (m_TextureId != 0)
		{
			glDeleteTextures(1, &m_TextureId);
			m_TextureId = 0;
		}
	}

	void Image2D::Update(uint32_t left, uint32_t top, uint32_t width, uint32_t height, const uint8_t* data)
	{
		// Clamp the update region to the image size
		left = std::min(left, m_Size.X);
		top = std::min(top, m_Size.Y);
		width = std::min(width, m_Size.X - left);
		height = std::min(height, m_Size.Y - top);

		const GLenum glFormat = ConvertImageFormatToGLFormat(m_Format);
		glTextureSubImage2D(m_TextureId, 0, left, top, width, height, glFormat, GL_UNSIGNED_BYTE, data);
	}

	std::unique_ptr<Image2D> Image2D::Clone() const
	{
		return CreateView(0, 0, m_Size.X, m_Size.Y);
	}

	std::unique_ptr<Image2D> Image2D::CreateView(const uint32_t left, const uint32_t top, const uint32_t width, const uint32_t height) const
	{
		const GLenum glInternalFormat = ConvertImageFormatToGLInternalFormat(m_Format);
		const GLenum glFormat = ConvertImageFormatToGLFormat(m_Format);
		const size_t dataSize = width * height * GetBytesByImageFormat(m_Format);

		// Query the texture data
		const auto data = std::make_unique<uint8_t[]>(dataSize);
		glGetTextureSubImage(m_TextureId, 0, left, top, 0, width, height, 1, glFormat, GL_UNSIGNED_BYTE, static_cast<GLsizei>(dataSize), data.get());

		// Generate new texture with the same data
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, glInternalFormat, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, glFormat, GL_UNSIGNED_BYTE, data.get());

		return std::unique_ptr<Image2D>(new Image2D(textureId, Math::Uint2{ width, height }, m_Format));
	}

	const Math::Uint2& Image2D::GetSize() const
	{
		return m_Size;
	}

	ImageFormat Image2D::GetFormat() const
	{
		return m_Format;
	}

	uint32_t Image2D::GetTextureId() const
	{
		return m_TextureId;
	}

	Image2D::Image2D(const uint32_t textureId, const Math::Uint2 size, const ImageFormat format) :
		m_TextureId(textureId),
		m_Size(size),
		m_Format(format)
	{
	}
}