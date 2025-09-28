// Project Name : DirectGL
// File Name    : DGL-OpenGLTexture.ixx
// Author       : Felix Busch
// Created Date : 2025/09/27

module;

#include <filesystem>
#include <cstdint>

#include <glad/gl.h>

export module DGL:OpenGLTexture;

import :Texture;

namespace DGL
{
	class OpenGLTexture : public Texture
	{
	public:

		[[nodiscard]] static std::unique_ptr<OpenGLTexture> Create(const std::filesystem::path& filepath);
		[[nodiscard]] static std::unique_ptr<OpenGLTexture> Create(uint32_t width, uint32_t height, uint32_t channels, const uint8_t* data);

		~OpenGLTexture() override;
		Math::Uint2 GetSize() const override;

		void Bind(uint32_t slot) const;

	private:

		explicit OpenGLTexture(GLuint textureId, const Math::Uint2& size, uint32_t channels);

		GLuint m_TextureId;
		Math::Uint2 m_Size;
		uint32_t m_Channels;

	};
}