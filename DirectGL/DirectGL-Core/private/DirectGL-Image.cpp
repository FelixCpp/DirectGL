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
	std::unique_ptr<Image2D> Image2D::CreateFromFile(const std::filesystem::path& filepath)
	{
		int width, height, channels;
		const std::unique_ptr<stbi_uc[], decltype(&stbi_image_free)> data(stbi_load(filepath.string().c_str(), &width, &height, &channels, 0), &stbi_image_free);
		if (data == nullptr)
		{
			Error(std::format("Failed to load image from file: {}", filepath.string()));
			return nullptr;
		}

		return CreateFromMemory(static_cast<uint32_t>(width), static_cast<uint32_t>(height), data.get());
	}

	std::unique_ptr<Image2D> Image2D::CreateFromMemory(const uint32_t width, const uint32_t height, const uint8_t* data)
	{
		const Math::Uint2 size = { width, height };

		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		
		return std::unique_ptr<Image2D>(new Image2D(textureId, size));
	}

	Image2D::~Image2D()
	{
		if (m_TextureId != 0)
		{
			glDeleteTextures(1, &m_TextureId);
		}
	}

	void Image2D::Update(uint32_t left, uint32_t top, uint32_t width, uint32_t height, const uint8_t* data)
	{
		// Clamp the update region to the image size
		left = std::min(left, m_Size.X);
		top = std::min(top, m_Size.Y);
		width = std::min(width, m_Size.X - left);
		height = std::min(height, m_Size.Y - top);

		glTextureSubImage2D(m_TextureId, 0, left, top, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	std::unique_ptr<Image2D> Image2D::Clone() const
	{
		return CreateView(0, 0, m_Size.X, m_Size.Y);
	}

	std::unique_ptr<Image2D> Image2D::CreateView(const uint32_t left, const uint32_t top, const uint32_t width, const uint32_t height) const
	{
		// Query the texture data
		const auto data = QueryPixelData();

		// Generate new texture with the same data
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_RGBA8, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.get());

		return std::unique_ptr<Image2D>(new Image2D(textureId, Math::Uint2{ width, height }));
	}

	std::unique_ptr<uint8_t[]> Image2D::QueryPixelData() const
	{
		const size_t dataSize = m_Size.X * m_Size.Y * 4;
		auto data = std::make_unique<uint8_t[]>(dataSize);
		glGetTextureImage(m_TextureId, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLsizei>(dataSize), data.get());
		return data;
	}

	const Math::Uint2& Image2D::GetSize() const
	{
		return m_Size;
	}

	uint32_t Image2D::GetTextureId() const
	{
		return m_TextureId;
	}

	Image2D::Image2D(const uint32_t textureId, const Math::Uint2 size) :
		m_TextureId(textureId),
		m_Size(size)
	{
	}
}