module;

#include <glad/gl.h>

#include <stdexcept>

module DGL;

namespace DGL
{
	constexpr GLint WrapModeIdToGlId(const TextureWrapModeId id)
	{
		switch (id)
		{
			case TextureWrapModeId::Repeat: return GL_REPEAT;
			case TextureWrapModeId::MirroredRepeat: return GL_MIRRORED_REPEAT;
			case TextureWrapModeId::ClampToEdge: return GL_CLAMP_TO_EDGE;
			case TextureWrapModeId::ClampToBorder: return GL_CLAMP_TO_BORDER;
			default: throw std::invalid_argument("Unsupported TextureWrapModeId.");
		}
	}

	constexpr GLint FilterModeIdToGlId(const TextureFilterModeId id)
	{
		switch (id)
		{
			case TextureFilterModeId::Nearest: return GL_NEAREST;
			case TextureFilterModeId::Linear: return GL_LINEAR;
			default: throw std::invalid_argument("Unsupported TextureFilterModeId.");
		}
	}

	std::unique_ptr<OpenGLTextureSampler> OpenGLTextureSampler::Create(const TextureWrapMode wrapMode, const TextureFilterMode filterMode)
	{
		GLuint samplerId = 0;
		glCreateSamplers(1, &samplerId);
		if (samplerId == 0)
		{
			Error("Failed to create texture sampler");
			return nullptr;
		}

		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_S, WrapModeIdToGlId(wrapMode.Horizontal));
		glSamplerParameteri(samplerId, GL_TEXTURE_WRAP_T, WrapModeIdToGlId(wrapMode.Vertical));
		glSamplerParameteri(samplerId, GL_TEXTURE_MIN_FILTER, FilterModeIdToGlId(filterMode.Minification));
		glSamplerParameteri(samplerId, GL_TEXTURE_MAG_FILTER, FilterModeIdToGlId(filterMode.Magnification));

		return std::unique_ptr<OpenGLTextureSampler>(new OpenGLTextureSampler(samplerId, wrapMode, filterMode));
	}

	OpenGLTextureSampler::~OpenGLTextureSampler()
	{
		if (m_SamplerId != 0)
		{
			glDeleteSamplers(1, &m_SamplerId);
		}
	}

	void OpenGLTextureSampler::SetWrapMode(const TextureWrapMode mode)
	{
		m_WrapMode = mode;
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_S, WrapModeIdToGlId(mode.Horizontal));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_WRAP_T, WrapModeIdToGlId(mode.Vertical));
	}

	TextureWrapMode OpenGLTextureSampler::GetWrapMode() const
	{
		return m_WrapMode;
	}

	void OpenGLTextureSampler::SetFilterMode(TextureFilterMode mode)
	{
		m_FilterMode = mode;
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MIN_FILTER, FilterModeIdToGlId(mode.Minification));
		glSamplerParameteri(m_SamplerId, GL_TEXTURE_MAG_FILTER, FilterModeIdToGlId(mode.Magnification));
	}

	TextureFilterMode OpenGLTextureSampler::GetFilterMode() const
	{
		return m_FilterMode;
	}

	void OpenGLTextureSampler::Bind(const uint32_t slot) const
	{
		glBindSampler(slot, m_SamplerId);
	}

	OpenGLTextureSampler::OpenGLTextureSampler(const GLuint id, const TextureWrapMode wrapMode, const TextureFilterMode filterMode):
		m_SamplerId(id),
		m_WrapMode(wrapMode),
		m_FilterMode(filterMode)
	{
	}
}