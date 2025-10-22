module;

#include <glad/gl.h>
#include <stb/stb_image.h>

#include <format>
#include <memory>

module DirectGL.Texture;

import DirectGL.Logging;

namespace DGL::Texture
{
	std::unique_ptr<Texture> Texture::Create(const Math::Uint2 size, const uint8_t* data)
	{
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_RGBA8, size.X, size.Y);
		glTextureSubImage2D(textureId, 0, 0, 0, size.X, size.Y, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return std::unique_ptr<Texture>(new Texture(size, textureId));
	}

	std::unique_ptr<Texture> Texture::Create(const std::filesystem::path& filepath)
	{
		int width, height, channels;
		const std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data(stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha), stbi_image_free);

		if (data == nullptr)
		{
			Logging::Error(std::format("Failed to load texture from file: {}", filepath.string()));
			return nullptr;
		}

		return Create(Math::Uint2(width, height), data.get());
	}

	Texture::~Texture()
	{
		if (m_TextureId != 0) glDeleteTextures(1, &m_TextureId);
	}

	Math::Uint2 Texture::GetSize() const
	{
		return m_Size;
	}

	GLuint Texture::GetRendererId() const
	{
		return m_TextureId;
	}

	Texture::Texture(const Math::Uint2 size, const GLuint textureId):
		m_TextureId(textureId),
		m_Size(size)
	{
	}
}
