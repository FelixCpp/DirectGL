module;

#include <glad/gl.h>

module DirectGL.Texture;

import Preconditions;

namespace DGL::Texture
{
	constexpr GLenum FilterModeToGlId(const TextureFilterModeId filterMode)
	{
		switch (filterMode)
		{
			case TextureFilterModeId::Nearest: return GL_NEAREST;
			case TextureFilterModeId::Linear: return GL_LINEAR;
			default: System::Error("Invalid TextureFilterModeId");
		}
	}

	constexpr GLenum WrapModeToGlId(const TextureWrapModeId wrapMode)
	{
		switch (wrapMode)
		{
			case TextureWrapModeId::Repeat: return GL_REPEAT;
			case TextureWrapModeId::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case TextureWrapModeId::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case TextureWrapModeId::ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: System::Error("Invalid TextureWrapModeId");
		}
	}

	std::unique_ptr<TextureSampler> TextureSampler::Create(const TextureFilterMode filterMode, const TextureWrapMode wrapMode)
	{
		GLuint samplerId = 0;
		glCreateSamplers(1, &samplerId);
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, FilterModeToGlId(filterMode.Minification));
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, FilterModeToGlId(filterMode.Magnification));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, WrapModeToGlId(wrapMode.Horizontal));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, WrapModeToGlId(wrapMode.Vertical));

		return std::unique_ptr<TextureSampler>(new TextureSampler(filterMode, wrapMode, samplerId));
	}

	TextureSampler::~TextureSampler()
	{
		if (m_SamplerId != 0) glDeleteSamplers(1, &m_SamplerId);
	}

	void TextureSampler::SetFilterMode(const TextureFilterMode mode)
	{
		if (m_FilterMode == mode)
		{
			return;
		}

		m_FilterMode = mode;
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MIN_FILTER, FilterModeToGlId(mode.Minification));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MAG_FILTER, FilterModeToGlId(mode.Magnification));
	}

	TextureFilterMode TextureSampler::GetFilterMode() const
	{
		return m_FilterMode;
	}

	void TextureSampler::SetWrapMode(const TextureWrapMode mode)
	{
		if (m_WrapMode == mode)
		{
			return;
		}

		m_WrapMode = mode;
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_S, WrapModeToGlId(mode.Horizontal));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_T, WrapModeToGlId(mode.Vertical));
	}

	TextureWrapMode TextureSampler::GetWrapMode() const
	{
		return m_WrapMode;
	}

	GLuint TextureSampler::GetRendererId() const
	{
		return m_SamplerId;
	}

	TextureSampler::TextureSampler(const TextureFilterMode filterMode, const TextureWrapMode wrapMode, const GLuint samplerId):
		m_FilterMode(filterMode),
		m_WrapMode(wrapMode),
		m_SamplerId(samplerId)
	{
	}
}