// Project Name : DirectGL-Texture
// File Name    : Texture-Texture.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <glad/gl.h>

#include <memory>

export module DirectGL.Texture:Texture;

import DirectGL.Math;

export namespace DGL::Texture
{
	class Texture
	{
	public:

		static std::unique_ptr<Texture> Create(Math::Uint2 size, const uint8_t* data);

		~Texture();

		Math::Uint2 GetSize() const;
		GLuint GetRendererId() const;

	private:

		explicit Texture(Math::Uint2 size, GLuint textureId);

		GLuint m_TextureId;
		Math::Uint2 m_Size;

	};
}