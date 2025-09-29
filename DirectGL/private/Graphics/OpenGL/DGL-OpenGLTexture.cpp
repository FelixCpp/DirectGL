module;

#include <format>

#include <stb/stb_image.h>
#include <glad/gl.h>

module DGL;

namespace DGL
{
	std::unique_ptr<OpenGLTexture> OpenGLTexture::Create(const std::filesystem::path& filepath)
	{
		int width, height, channels;

		const auto data = std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>(
			stbi_load(filepath.string().c_str(), &width, &height, &channels, 0),
			&stbi_image_free
		);

		if (data == nullptr)
		{
			Error(std::format("Failed to load texture: '{}'", filepath.string()));
			return nullptr;
		}

		return Create(static_cast<uint32_t>(width), static_cast<uint32_t>(height), static_cast<uint32_t>(channels), data.get());
	}

	std::unique_ptr<OpenGLTexture> OpenGLTexture::Create(const uint32_t width, const uint32_t height, const uint32_t channels, const uint8_t* data)
	{
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, channels == 4 ? GL_RGBA8 : GL_RGB8, width, height);
		glTextureSubImage2D(textureId, 0, 0, 0, width, height, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

		return std::unique_ptr<OpenGLTexture>(new OpenGLTexture(textureId, {width, height}, channels));
	}

	OpenGLTexture::~OpenGLTexture()
	{
		if (m_TextureId != 0)
		{
			glDeleteTextures(1, &m_TextureId);
		}
	}

	Math::Uint2 OpenGLTexture::GetSize() const
	{
		return m_Size;
	}

	void OpenGLTexture::Bind(const uint32_t slot) const
	{
		glBindTextureUnit(slot, m_TextureId);
	}

	OpenGLTexture::OpenGLTexture(const GLuint textureId, const Math::Uint2& size, const uint32_t channels)
		: m_TextureId(textureId), m_Size(size), m_Channels(channels)
	{
	}
}