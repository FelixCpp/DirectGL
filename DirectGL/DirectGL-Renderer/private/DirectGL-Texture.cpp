module;

#include <glad/gl.h>

module DirectGL.Renderer;

namespace DGL::Renderer
{
	std::unique_ptr<Texture> Texture::Create(const Math::Uint2 size, const uint8_t* data)
	{
		GLuint textureId = 0;
		glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
		glTextureStorage2D(textureId, 1, GL_RGBA8, size.X, size.Y);
		glTextureSubImage2D(textureId, 0, 0, 0, size.X, size.Y, GL_RGBA, GL_UNSIGNED_BYTE, data);

		return std::unique_ptr<Texture>(new Texture(size, textureId));
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
