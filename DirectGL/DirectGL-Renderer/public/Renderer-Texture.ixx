// Project Name : DirectGL-Renderer
// File Name    : Renderer-Texture.ixx
// Author       : Felix Busch
// Created Date : 2025/10/11

module;

#include <glad/gl.h>

#include <memory>
//#include <span>

export module DirectGL.Renderer:Texture;

import DirectGL.Math;

export namespace DGL::Renderer
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